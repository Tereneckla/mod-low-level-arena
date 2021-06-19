/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 * Copyright (C) 2021+ WarheadCore <https://github.com/WarheadCore>
 */

#include "LowLevelArena.h"
#include "Log.h"
#include "ScriptMgr.h"
#include "Config.h"
#include "Chat.h"
#include "Player.h"
#include "ScriptedGossip.h"

#ifndef STRING_VIEW_FMT
#define STRING_VIEW_FMT "%.*s"
#define STRING_VIEW_FMT_ARG(str) static_cast<int>((str).length()), (str).data()
#endif

//class LowLevelArena_BG : public BGScript
//{
//public:
//    LowLevelArena_BG() : BGScript("LowLevelArena_BG") { }
//
//    void OnBattlegroundEnd(Battleground* bg, TeamId winnerTeamId) override
//    {
//        if (!sConfigMgr->GetOption<bool>("LLA.Enable", false))
//            return;
//
//        // Not reward for bg or rated arena
//        if (bg->isBattleground() || bg->isRated())
//        {
//            return;
//        }
//
//        sLLA->Reward(bg, winnerTeamId);
//    }
//};

class LowLevelArena_Command : public CommandScript
{
public:
    LowLevelArena_Command() : CommandScript("LowLevelArena_Command") {}

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> llaQueueCommandTable =
        {
            { "solo",   SEC_PLAYER, false,  &HandleLLAQueueSolo, "" },
            { "group",  SEC_PLAYER, false,  &HandleLLAQueueGroup, "" }
        };

        static std::vector<ChatCommand> llaCommandTable =
        {
            { "queue",  SEC_PLAYER, false, nullptr, "", llaQueueCommandTable }
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "lla",    SEC_PLAYER, false, nullptr, "", llaCommandTable }
        };

        return commandTable;
    }

    static bool AddQueue(Player* player, std::string_view arenaType, bool joinAsGroup)
    {
        ChatHandler handler(player->GetSession());

        if (!sConfigMgr->GetOption<bool>("LLA.Enable", false))
        {
            handler.PSendSysMessage("> Module disable!");
            return true;
        }

        if (arenaType.empty())
        {
            handler.PSendSysMessage("> Arena type '" STRING_VIEW_FMT "' is incorrect. Please use 2v2, 3v3, 5v5", STRING_VIEW_FMT_ARG(arenaType));
            return false;
        }

        // Try join queue
        if (arenaType == "2v2")
        {
            // Join 2v2
            sLLA->AddQueue(player, ARENA_TYPE_2v2, joinAsGroup);
        }
        else if (arenaType == "3v3")
        {
            // Join 3v3
            sLLA->AddQueue(player, ARENA_TYPE_3v3, joinAsGroup);
        }
        else if (arenaType == "5v5")
        {
            // Join 5v5
            sLLA->AddQueue(player, ARENA_TYPE_5v5, joinAsGroup);
        }
        else
        {
            handler.PSendSysMessage("> Arena type '" STRING_VIEW_FMT "' is incorrect. Please use 2v2, 3v3, 5v5", STRING_VIEW_FMT_ARG(arenaType));
            return false;
        }

        return true;
    }

    static bool HandleLLAQueueSolo(ChatHandler* handler, const char* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        if (!AddQueue(player, args, false))
        {
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }

    static bool HandleLLAQueueGroup(ChatHandler* handler, const char* args)
    {
        Player* player = handler->GetSession()->GetPlayer();
        if (!player)
            return false;

        if (!AddQueue(player, args, true))
        {
            handler->SetSentErrorMessage(true);
            return false;
        }

        return true;
    }
};

class LowLevelArena_World : public WorldScript
{
public:
    LowLevelArena_World() : WorldScript("LowLevelArena_World") { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        // Add conigs options configiration
    }
};

// Group all custom scripts
void AddSC_LowLevelArena()
{
    //new LowLevelArena_BG();
    new LowLevelArena_Command();
    //new LowLevelArena_World();
}
