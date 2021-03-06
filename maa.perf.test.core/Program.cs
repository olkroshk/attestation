﻿using System;
using System.Net;
using System.Threading;
using System.Threading.Tasks;
using maa.perf.test.core.Utils;
using CommandLine;
using maa.perf.test.core.Maa;
using maa.perf.test.core.Authentication;

namespace maa.perf.test.core
{
    class Program
    {
        public class Options
        {
            [Option('v', "verbose", Required = false, HelpText = "Set output to verbose messages.")]
            public bool Verbose { get; set; }

            [Option('p', "provider", Required = false, HelpText = "Attestation provider DNS name.")]
            public string AttestationProvider { get; set; }

            [Option('q', "quote", Required = false, HelpText = "Enclave info file containing the SGX quote.")]
            public string EnclaveInfoFile { get; set; }

            [Option('c', "connections", Required = false, HelpText = "Number of simultaneous connections (and calls) to the MAA service.")]
            public long  SimultaneousConnections { get; set; }

            [Option('r', "rps", Required = false, HelpText = "Target RPS.")]
            public long TargetRPS { get; set; }

            public Options()
            {
                Verbose = true;
                AttestationProvider = "sharedeus.eus.test.attest.azure.net";
                EnclaveInfoFile = "./Quotes/enclave.info.release.json";
                SimultaneousConnections = 16;
                TargetRPS = 10;
            }
        }

        private const long TargetRPS = 10;
        private const long ParallelConnections = 20;
        private Options _options;
        private EnclaveInfo _enclaveInfo;
        private MaaService _maaService;

        static void Main(string[] args)
        {
            ServicePointManager.DefaultConnectionLimit = 1024 * 32;

            Parser.Default.ParseArguments<Options>(args)
                .WithParsed<Options>(o =>
                {
                    new Program(o).RunAsync().Wait();
                });
        }

        public Program(Options options)
        {
            _options = options;

            Tracer.CurrentTracingLevel = _options.Verbose ? TracingLevel.Verbose : TracingLevel.Warning;
            _enclaveInfo = EnclaveInfo.CreateFromFile(_options.EnclaveInfoFile);
            _maaService = new MaaService(_options.AttestationProvider);

            Tracer.TraceInfo($"Attestation Provider     : {_options.AttestationProvider}");
            Tracer.TraceInfo($"Enclave Info File        : {_options.EnclaveInfoFile}");
            Tracer.TraceInfo($"Simultaneous Connections : {_options.SimultaneousConnections}");
            Tracer.TraceInfo($"Target RPS               : {_options.TargetRPS}");
        }

        public async Task RunAsync()
        {
            await Authentication.Authentication.AcquireAccessTokenAsync("microsoft.com", false);

            AsyncFor myFor = new AsyncFor(_options.TargetRPS, "MAA SGX Attest");
            myFor.PerSecondMetricsAvailable += new ConsoleMetricsHandler().MetricsAvailableHandler;
            myFor.PerSecondMetricsAvailable += new CsvFileMetricsHandler().MetricsAvailableHandler;
            await myFor.For(TimeSpan.MaxValue, _options.SimultaneousConnections, CallAttestSgx);
        }

        public async Task<double> CallAttestSgx()
        {
            var serviceJwtToken = await _maaService.AttestOpenEnclaveAsync(_enclaveInfo.GetMaaBody());
            return await Task.FromResult(0.0);
        }

    }
}
