#include "yet_server.h"
#include <asio.hpp>
#include "yet_config.h"
#include "yet_http_flv_server.h"
#include "yet_rtmp_server.h"
#include "yet_group.h"

namespace yet {

Server::Server(const std::string &rtmp_listen_ip, uint16_t rtmp_listen_port,
               const std::string &http_flv_listen_ip, uint16_t http_flv_listen_port)
  : rtmp_server_(std::make_shared<RtmpServer>(io_ctx_, rtmp_listen_ip, rtmp_listen_port, this))
  , http_flv_server_(std::make_shared<HttpFlvServer>(io_ctx_, http_flv_listen_ip, http_flv_listen_port, this))
{
  YET_LOG_DEBUG("[{}] [lifecycle] new Server.", (void *)this);
}

Server::~Server() {
  YET_LOG_DEBUG("[{}] [lifecycle] delete Server.", (void *)this);
}

void Server::run_loop() {

  rtmp_server_->start();
  http_flv_server_->start();
  io_ctx_.run();
}

void Server::dispose() {
  rtmp_server_->dispose();
  http_flv_server_->dispose();

  for (auto &it : stream_name_2_group_) {
    it.second->dispose();
  }

  io_ctx_.stop();
}

GroupPtr Server::get_or_create_group(const std::string &app_name, const std::string &stream_name) {
  // CHEFTODO manage app_name with group
  auto iter = stream_name_2_group_.find(stream_name);
  if (iter != stream_name_2_group_.end()) {
    return iter->second;
  }

  auto group = std::make_shared<Group>(io_ctx_, app_name, stream_name);
  stream_name_2_group_[stream_name] = group;
  return group;
}

GroupPtr Server::get_group(const std::string &stream_name) {
  auto iter = stream_name_2_group_.find(stream_name);
  if (iter != stream_name_2_group_.end()) {
    return iter->second;
  }
  return nullptr;
}

}
