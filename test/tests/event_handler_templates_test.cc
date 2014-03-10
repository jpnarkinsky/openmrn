#include <stdio.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "os/os.h"

#include "nmranet/EventHandlerTemplates.hxx"
#include "nmranet/NMRAnetEventTestHelper.hxx"

using ::testing::_;
using ::testing::Mock;
using ::testing::StrictMock;

namespace NMRAnet {

class SimpleEventProxy : public ProxyEventHandler {
 public:
  SimpleEventProxy(NMRAnetEventHandler* handler)
      : handler_(handler) {}

  virtual void HandlerFn(EventHandlerFunction fn,
                                 EventReport* event,
                                 Notifiable* done) {
    (handler_->*fn)(event, done);
  }

 private:
  NMRAnetEventHandler* handler_;
};

TEST(EventProxy, TestHandleEvent) {
  StrictMock<MockEventHandler> inner_handler;
  SimpleEventProxy proxy(&inner_handler);
  EXPECT_CALL(inner_handler, HandleEventReport(_, _)).Times(1);
  proxy.HandleEventReport(NULL, NULL);
}

TEST(EventProxy, ConsumerRange) {
  StrictMock<MockEventHandler> inner_handler;
  SimpleEventProxy proxy(&inner_handler);
  EXPECT_CALL(inner_handler, HandleConsumerRangeIdentified(_, _)).Times(1);
  proxy.HandleConsumerRangeIdentified(NULL, NULL);
}

TEST(DummyTest, Success) {
  int i = 34;
  EXPECT_EQ(34, i);
}

}  // namespace NMRAnet

int appl_main(int argc, char* argv[]) {
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
