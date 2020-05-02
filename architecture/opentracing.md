分布式链路日志追踪



定义:

1. Trace: The desciption of a transaction as it movies through a distributed system.
2. Span: A named ,timed operation representing a piece of the workflow, Span accept Key:value as well as fine-grained, timestamped, structured logs attaced to particular span instance
3. Span Context: Trace infomation that accompanies the distributed transaction, including when it passes service to service over the network or through the message bus. The span context contains the trace identifier, span identifier, and any other data that the tracing system needs to propagate to the system downstram service





-   [异构系统链路追踪—滴滴 trace 实践](https://v.qq.com/x/page/p0546gars01.html)
-   [opentracing tutorial](https://github.com/yurishkuro/opentracing-tutorial)

