﻿using System;

namespace maa.perf.test.core.Utils
{
    class ConsoleMetricsHandler
    {
        public ConsoleMetricsHandler()
        {
        }

        public void MetricsAvailableHandler(IntervalMetrics metrics)
        {
            MetricsAvailableHandlerSingleLine(metrics);
        }

        public void MetricsAvailableHandlerSingleLine(IntervalMetrics metrics)
        {
            Console.WriteLine(
                "Resource: {0,-30}, Test: {1,-20}, Time: {2,-22}, RPS: {3,5:f0}, Average:{4,5}, P50:{5,5}, P90:{6,5}, P95:{7,5}, P99:{8,5}",
                metrics.ResourceDescription, // 0
                metrics.TestDescription,     // 1
                metrics.EndTime.ToString(),  // 2
                metrics.RPS,                 // 3
                metrics.AverageLatencyMS,    // 4
                metrics.Percentile50,        // 5
                metrics.Percentile90,        // 6
                metrics.Percentile95,        // 7
                metrics.Percentile99);       // 8
        }
    }
}
