#include <iostream>

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "ping_DCPS.hpp"
#include "pong_DCPS.hpp"
#include <dds/domain/DomainParticipant.hpp>
#include <dds/sub/Subscriber.hpp>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <algorithm>
#include <thread>
#include <iostream>

void HandleSample(const dds::sub::Sample<test::ping::Ping>& sample)
{
  if (sample.info().valid())
    {
      std::cout << "Ping: " << sample.data().ing() << std::endl;
    }
  else
    {
      std::cout << "Sample invalid" << std::endl;
    }
}


int main(int argc, char *argv[])
{
  auto domainParticipant = dds::domain::DomainParticipant(0);
  auto subscriber = dds::sub::Subscriber(domainParticipant);

  auto pingTopic = dds::topic::Topic<test::ping::Ping>(
      domainParticipant,
      "TestPing");
  auto pingReaderQos = subscriber.default_datareader_qos();
  pingReaderQos
    << dds::core::policy::Durability::TransientLocal()
    << dds::core::policy::Reliability::Reliable();
  auto pingReader = dds::sub::DataReader<test::ping::Ping>(
      subscriber,
      pingTopic,
      pingReaderQos);

  // Clear reader queue of any old samples
  pingReader.wait_for_historical_data(dds::core::Duration::infinite());
  pingReader.take();

  auto readCondition = dds::sub::cond::ReadCondition(
      pingReader,
      dds::sub::status::DataState::new_data());

  auto waitSet = dds::core::cond::WaitSet();
  waitSet.attach_condition(readCondition);

  bool ong = false;
  std::unique_ptr<test::pong::Pong> sample(new test::pong::Pong);


  while(true)
  {

    waitSet.wait();
    const auto samples = pingReader.select().take();
    std::for_each(
        samples.begin(),
        samples.end(),
        HandleSample);
  }

  return EXIT_SUCCESS;

}
