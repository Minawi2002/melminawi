//
// Created by Manuel on 28.01.2021.
// Updates by UNOrganized on 11.12.2023.
//

#include "client_request.h"
#include "play_card_request.h"
#include "draw_card_request.h"
#include "call_uno_request.h"
#include "join_game_request.h"
#include "start_game_request.h"
#include "new_round_request.h"
#include "pick_a_color_request.h"
#include "player_left_request.h"

#include <iostream>

// for deserialization
const std::unordered_map<std::string, RequestType> client_request::_string_to_request_type = {
        {"join_game", RequestType::join_game },
        {"start_game", RequestType::start_game},
        {"new_round", RequestType::new_round},
        {"play_card", RequestType::play_card},
        {"draw_card", RequestType::draw_card},
        {"call_uno", RequestType::call_uno},
        {"pick_a_color", RequestType::pick_a_color},
        {"player_left", RequestType::player_left}
};
// for serialization
const std::unordered_map<RequestType, std::string> client_request::_request_type_to_string = {
        { RequestType::join_game, "join_game" },
        { RequestType::start_game, "start_game"},
        { RequestType::new_round, "new_round"},
        { RequestType::play_card, "play_card"},
        { RequestType::draw_card, "draw_card"},
        { RequestType::call_uno, "call_uno"},
        { RequestType::pick_a_color, "pick_a_color"},
        { RequestType::player_left, "player_left"}
};

// protected constructor. only used by subclasses
client_request::client_request(client_request::base_class_properties props) :
        _type(props._type),
        _req_id(props._req_id),
        _player_id(props._player_id),
        _game_id(props._game_id)
{ }


// used by subclasses to retrieve information from the json stored by this superclass
client_request::base_class_properties client_request::extract_base_class_properties(const rapidjson::Value& json) {
    if (json.HasMember("player_id") && json.HasMember("game_id") && json.HasMember("req_id")) {
        std::string player_id = json["player_id"].GetString();
        std::string game_id = json["game_id"].GetString();
        std::string req_id = json["req_id"].GetString();
        return create_base_class_properties(
                client_request::_string_to_request_type.at(json["type"].GetString()),
                req_id,
                player_id,
                game_id
        );
    }
    else
    {
        throw UnoException("Client Request did not contain player_id or game_id");
    }
}

client_request::base_class_properties client_request::create_base_class_properties(
        RequestType type,
        std::string req_id,
        std::string& player_id,
        std::string& game_id)
{
    client_request::base_class_properties res;
    res._player_id = player_id;
    res._game_id = game_id;
    res._req_id = req_id;
    res._type = type;
    return res;
}


void client_request::write_into_json(rapidjson::Value &json,
                                     rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    rapidjson::Value type_val(_request_type_to_string.at(this->_type).c_str(), allocator);
    json.AddMember("type", type_val, allocator);

    rapidjson::Value player_id_val(_player_id.c_str(), allocator);
    json.AddMember("player_id", player_id_val, allocator);

    rapidjson::Value game_id_val(_game_id.c_str(), allocator);
    json.AddMember("game_id", game_id_val, allocator);

    rapidjson::Value req_id_val(_req_id.c_str(), allocator);
    json.AddMember("req_id", req_id_val, allocator);
}

client_request* client_request::from_json(const rapidjson::Value &json) {
    if (json.HasMember("type") && json["type"].IsString()) {
        const std::string type = json["type"].GetString();
        const RequestType request_type = client_request::_string_to_request_type.at(type);

        // Check which type of request it is and call the respective from_json constructor
        if (request_type == RequestType::join_game) {
            return join_game_request::from_json(json);
        }
        else if (request_type == RequestType::start_game) {
            return start_game_request::from_json(json);
        }
        else if (request_type == RequestType::new_round) {
            return new_round_request::from_json(json);
        }
        else if (request_type == RequestType::play_card) {
            return play_card_request::from_json(json);
        }
        else if (request_type == RequestType::draw_card) {
            return draw_card_request::from_json(json);
        }
        else if (request_type == RequestType::call_uno) {
            return call_uno_request::from_json(json);
        }
        else if (request_type == RequestType::pick_a_color){
            return pick_a_color_request::from_json(json);
        }
        else if (request_type == RequestType::player_left){
            return player_left_request::from_json(json);
        } else {
            throw UnoException("Encountered unknown ClientRequest type " + type);
        }
    }
    throw UnoException("Could not determine type of ClientRequest. JSON was:\n" + json_utils::to_string(&json));
}


std::string client_request::to_string() const {
    return "client_request of type " + client_request::_request_type_to_string.at(_type) + " for playerId " + _player_id + " and gameId " + _game_id;
}






