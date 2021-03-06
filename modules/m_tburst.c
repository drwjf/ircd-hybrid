/*
 *  ircd-hybrid: an advanced, lightweight Internet Relay Chat Daemon (ircd)
 *
 *  Copyright (c) 2002-2017 ircd-hybrid development team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
 *  USA
 */

/*! \file m_tburst.c
 * \brief Includes required functions for processing the TBURST command.
 * \version $Id$
 */

#include "stdinc.h"
#include "client.h"
#include "ircd.h"
#include "send.h"
#include "modules.h"
#include "hash.h"
#include "server.h"
#include "conf.h"
#include "parse.h"


/*! \brief TBURST command handler
 *
 * \param source_p Pointer to allocated Client struct from which the message
 *                 originally comes from.  This can be a local or remote client.
 * \param parc     Integer holding the number of supplied arguments.
 * \param parv     Argument vector where parv[0] .. parv[parc-1] are non-NULL
 *                 pointers.
 * \note Valid arguments for this command are:
 *      - parv[0] = command
 *      - parv[1] = channel timestamp
 *      - parv[2] = channel name
 *      - parv[3] = topic timestamp
 *      - parv[4] = topic setter
 *      - parv[5] = topic (can be an empty string)
 */
static int
ms_tburst(struct Client *source_p, int parc, char *parv[])
{
  struct Channel *chptr = NULL;
  int accept_remote = 0;
  uintmax_t remote_channel_ts = strtoumax(parv[1], NULL, 10);
  uintmax_t remote_topic_ts = strtoumax(parv[3], NULL, 10);
  const char *topic = parv[5];
  const char *setby = parv[4];

  /*
   * Do NOT test parv[5] for an empty string and return if true!
   * parv[5] CAN be an empty string, i.e. if the other side wants
   * to unset our topic.  Don't forget: an empty topic is also a
   * valid topic.
   */


  if ((chptr = hash_find_channel(parv[2])) == NULL)
    return 0;

  /*
   * The logic for accepting and rejecting channel topics was
   * always a bit hairy, so now we got exactly 3 cases where
   * we would accept a topic
   *
   * Case 1:
   *        A services client/server wants to set a topic
   * Case 2:
   *        The TS of the remote channel is older than ours
   * Case 3:
   *        The TS of the remote channel is equal to ours AND
   *        the TS of the remote topic is newer than ours
   */
  if (HasFlag(source_p, FLAGS_SERVICE))
    accept_remote = 1;
  else if (remote_channel_ts < chptr->creationtime)
    accept_remote = 1;
  else if (remote_channel_ts == chptr->creationtime)
    if (remote_topic_ts > chptr->topic_time)
      accept_remote = 1;

  if (accept_remote)
  {
    int topic_differs = strncmp(chptr->topic, topic, sizeof(chptr->topic) - 1);
    int hidden_server = (ConfigServerHide.hide_servers || IsHidden(source_p));

    channel_set_topic(chptr, topic, setby, remote_topic_ts, 0);

    sendto_server(source_p, CAPAB_TBURST, 0, ":%s TBURST %s %s %s %s :%s",
                  source_p->id, parv[1], parv[2], parv[3], setby, topic);

    if (topic_differs)
    {
      if (!IsClient(source_p))
        sendto_channel_local(NULL, chptr, 0, 0, 0, ":%s TOPIC %s :%s",
                             hidden_server ? me.name : source_p->name,
                             chptr->name, chptr->topic);
      else
        sendto_channel_local(NULL, chptr, 0, 0, 0, ":%s!%s@%s TOPIC %s :%s",
                             source_p->name, source_p->username, source_p->host,
                             chptr->name, chptr->topic);
    }
  }

  return 0;
}

static struct Message tburst_msgtab =
{
  .cmd = "TBURST",
  .args_min = 6,
  .args_max = MAXPARA,
  .handlers[UNREGISTERED_HANDLER] = m_ignore,
  .handlers[CLIENT_HANDLER] = m_ignore,
  .handlers[SERVER_HANDLER] = ms_tburst,
  .handlers[ENCAP_HANDLER] = m_ignore,
  .handlers[OPER_HANDLER] = m_ignore
};

static void
module_init(void)
{
  mod_add_cmd(&tburst_msgtab);
  add_capability("TBURST", CAPAB_TBURST);
}

static void
module_exit(void)
{
  mod_del_cmd(&tburst_msgtab);
  delete_capability("TBURST");
}

struct module module_entry =
{
  .version = "$Revision$",
  .modinit = module_init,
  .modexit = module_exit,
};
