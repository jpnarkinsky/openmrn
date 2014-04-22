// Basic tests.

#include "utils/async_if_test_helper.hxx"

#include "nmranet/NMRAnetEventRegistry.hxx"
#include "nmranet/NMRAnetIf.hxx"

namespace NMRAnet {

TEST_F(AsyncNodeTest, Setup) {}

TEST_F(AsyncNodeTest, WriteMessageSync) {
  // We write a message using the WriteFlow class directly into the interface.
  expect_packet(":X195B422AN0102030405060708;");
  event_write_helper1.WriteAsync(node_, If::MTI_EVENT_REPORT, WriteHelper::global(),
                                 EventIdToBuffer(0x0102030405060708ULL),
                                 get_notifiable());
}

TEST_F(AsyncNodeTest, WriteMessageASync) {
  // We write a message using the WriteFlow class asynchronously.
  expect_packet(":X195B422AN0102030405060708;");
  event_write_helper1.WriteAsync(node_, If::MTI_EVENT_REPORT, WriteHelper::global(),
                                 EventIdToBuffer(0x0102030405060708ULL),
                                 get_notifiable());
  wait_for_notification();
}

/** This is disabled because AME frmaes are not yet supported. 
 * @TODO(balazs.racz): implement AME support and reenable.
 */
TEST_F(AsyncNodeTest, DISABLED_ReadMessageAndReply) {
  // We send an alias mapping enquiry frame and expect the node ID back.
  expect_packet(":X1070122AN02010d000003;");
  send_packet(  ":X10702001N;");
}

}  // namespace NMRAnet
