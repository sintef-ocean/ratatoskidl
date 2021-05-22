#include <iostream>

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "pingpong/ping_DCPS.hpp"
#include "pingpong/pong_DCPS.hpp"
#include <dds/domain/DomainParticipant.hpp>
#include <dds/pub/Publisher.hpp>
//#include <dds/sub/Subscriber.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <thread>
#include <iostream>

int main(int argc, char *argv[])
{
  auto domainParticipant = dds::domain::DomainParticipant(0);
  auto publisher = dds::pub::Publisher(domainParticipant);

  auto pingTopic = dds::topic::Topic<test::ping::Ping>(
      domainParticipant,
      "TestPing");

  auto pingWriterQos = publisher.default_datawriter_qos();
  pingWriterQos << dds::core::policy::Durability::TransientLocal();
  auto pingWriter = dds::pub::DataWriter<test::ping::Ping>(
      publisher,
      pingTopic,
      pingWriterQos);

  int ing = 0;
  std::unique_ptr<test::ping::Ping> sample(new test::ping::Ping);

  while(true)
  {

    sample->ing() = ing++;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    pingWriter << *sample;
    std::cout << "Ping: " << ing - 1 << std::endl;

  }

  return EXIT_SUCCESS;

}
