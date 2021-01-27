/***
 *  @project: Firestorm Freelance
 *  @author: Meltie2013 (github) aka Lilcrazy
 *  @copyright: 2017
 */

#include "ScriptMgr.h"
#include "Config.h"
#include "World.h"
#include "WorldSession.h"
#include "Chat.h"
#include "Player.h"

// Check to see if the player is attempting to multi-box
class multi_login_check : public PlayerScript
{
public:
    multi_login_check() : PlayerScript("multi_login_check") { }

	void OnLogin(Player* player)
    {
        uint32 CountLimit = sConfigMgr->GetIntDefault("Disallow.Multiple.Client", 0);
	if (CountLimit>0)
        {
	// There is a Limit of Client
	    if (sConfigMgr->GetBoolDefault("Disallow.Multiple.Client.Announce", true))
            {	
               	std::ostringstream stream;
		stream << "This Server Max Account of Same IP Is:" << CountLimit;
		ChatHandler(player->GetSession()).SendSysMessage(stream.str().c_str());
            }	
            SessionMap sessions = sWorld->GetAllSessions();
            uint32 count = 1;
	    for (SessionMap::iterator itr = sessions.begin(); itr != sessions.end(); ++itr)
            {
                if (Player* login = itr->second->GetPlayer())
                {
                    if (player != login)
                    {
                        // If Remote Address matches, remove the player from the world
                        if (player->GetSession()->GetRemoteAddress() == login->GetSession()->GetRemoteAddress())
                            {
				if (++count > CountLimit)
				{
				    player->GetSession()->KickPlayer();
				}
			    }
                    }
                }
            }
        }
	}
};

class multi_login_check_world : public WorldScript
{
public:
	multi_login_check_world() : WorldScript("multi_login_check_world") { }

	void OnBeforeConfigLoad(bool reload) override
	{
		if (!reload) {
			std::string conf_path = _CONF_DIR;
			std::string cfg_file = conf_path + "/mod_multilogin.conf";
#ifdef WIN32
			cfg_file = "mod_multilogin.conf";
#endif
			std::string cfg_def_file = cfg_file + ".dist";
			sConfigMgr->LoadMore(cfg_def_file.c_str());

			sConfigMgr->LoadMore(cfg_file.c_str());
		}
	}
};

void AddMultiLoginCheckScripts()
{
    new multi_login_check_world;
    new multi_login_check;
}
