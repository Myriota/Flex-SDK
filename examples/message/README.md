# Schedule Message Example

This example demonstrates how to schedule messages for satellite
transmission. The `SendMessage` job will schedule `MESSAGES_PER_DAY`
messages per day. The `FLEX_MessageSchedule` function pushes each
message to the queue. The return value of `FLEX_MessageSchedule`
indicates the load of the queue. A return value greater than one
indicates that the queue is overloaded and that messages may begin to be
dropped. The message will be transmitted when a satellite is overhead
and the satellite passes can be checked from Access Times in Device
Manager.
