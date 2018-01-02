#include <iostream>

#include "PeerList.h"
#include "CommandFactory.h"
#include "RaftFollowerState.h"
#include "JsonTools.h"

RaftFollowerState::RaftFollowerState(boost::asio::io_service& ios,
                                       Storage& s,
                                       CommandFactory& cf,
                                       ApiCommandQueue& pq,
                                       PeerList& ps,
                                     function<string(const string&)> rh,
                                     function<void(unique_ptr<RaftState>)> set_next)
        : RaftState(ios, s, cf, pq, ps, rh, set_next),
          heartbeat_timer_(ios_,
                           boost::posix_time::milliseconds(RaftState::raft_election_timeout_interval_min_milliseconds))
{
    std::cout << "          I am Follower" << std::endl;

    heartbeat_timer_.async_wait(boost::bind(&RaftFollowerState::heartbeat_timer_expired, this));
}


unique_ptr<RaftState> RaftFollowerState::handle_request(const string& request, string& response)
{
    auto pt = pt_from_json_string(request);

    unique_ptr<Command> command = command_factory_.get_command(pt, *this);
    response = pt_to_json_string(command->operator()());

    if (next_state_ != nullptr)
        return std::move(next_state_);

    return nullptr;
}


void RaftFollowerState::heartbeat_timer_expired()
{
    std::cout << "Starting Leader election" << std::endl;

    set_next_state_candidate();

    set_next_state_(std::move(next_state_));

    //
    //raft_state_ = std::move(raft_state_->next_state_);
}

void RaftFollowerState::rearm_heartbeat_timer() // Use with flag to stop it when transitioned to another state
{
    heartbeat_timer_.expires_from_now(
            boost::posix_time::milliseconds(RaftState::raft_election_timeout_interval_min_milliseconds));
}
