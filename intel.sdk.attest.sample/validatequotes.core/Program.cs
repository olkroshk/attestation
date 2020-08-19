﻿using System;
using System.IO;
using System.Threading.Tasks;
using validatequotes.Helpers;

namespace validatequotes
{
    public class Program
    {
        private string fileName;
        private string attestDnsName;
        private bool includeDetails;
        private bool dumpJWTOnly;

        public static void Main(string[] args)
        {
            Task.WaitAll(new Program(args).RunAsync());
        }

        public Program(string[] args)
        {
            this.fileName = args.Length > 0 ? args[0] : (Directory.GetCurrentDirectory().Contains("bin", StringComparison.InvariantCultureIgnoreCase) ? "../../../../genquotes/quotes/enclave.info.release.json" : "../genquotes/quotes/enclave.info.release.json");
            this.attestDnsName = args.Length > 1 ? args[1] : "shareduks.uks.attest.azure.net";
            this.includeDetails = true;
            this.dumpJWTOnly = false;
            if (args.Length > 2)
            {
                bool.TryParse(args[2], out this.includeDetails);
            }

            if (args.Length > 3)
            {
                bool.TryParse(args[3], out this.dumpJWTOnly);
            }

            if (args.Length < 3)
            {
                Logger.WriteBanner($"USAGE");
                Logger.WriteLine($"Usage: dotnet validatequotes.core.dll <JSON file name> <attest DNS name> <include details bool> <dump-JWT-only>");
                Logger.WriteLine($"Usage: dotnet run                     <JSON file name> <attest DNS name> <include details bool> <dump-JWT-only>");
                Logger.WriteLine($" - validates remote attestation quotes generated by genquote application");
                Logger.WriteLine($" - validates via calling the OE attestation endpoint on the MAA service");
            }

            Logger.WriteBanner($"PARAMETERS FOR THIS RUN");
            Logger.WriteLine($"Validating filename                : {this.fileName}");
            Logger.WriteLine($"Using attestation provider         : {this.attestDnsName}");
            Logger.WriteLine($"Including details                  : {this.includeDetails}");
            Logger.WriteLine($"Dump JWT only                      : {this.dumpJWTOnly}");
        }

        public async Task RunAsync()
        {
            // Fetch file
            var enclaveInfo = EnclaveInfo.CreateFromFile(this.fileName);

            // Send to service for attestation
            var maaService = new MaaService(this.attestDnsName);
            var serviceJwtToken = await maaService.AttestOpenEnclaveAsync(enclaveInfo.GetMaaBody());

            // Dump JWT only?
            if (this.dumpJWTOnly)
            {
                var jwtBody = JoseHelper.ExtractJosePart(serviceJwtToken, 1);
                Console.WriteLine($"{jwtBody.ToString()}");
                Directory.CreateDirectory("./maa-jwt");
                File.WriteAllText($"./maa-jwt/{DateTime.Now.Ticks}.json", jwtBody.ToString());
            }
            else
            {
                // Analyze results
                Logger.WriteBanner("VALIDATING MAA JWT TOKEN - BASICS");
                JwtValidationHelper.ValidateMaaJwt(attestDnsName, serviceJwtToken, this.includeDetails);

                Logger.WriteBanner("VALIDATING MAA JWT TOKEN - MATCHES CLIENT ENCLAVE INFO");
                enclaveInfo.CompareToMaaServiceJwtToken(serviceJwtToken, this.includeDetails);
            }

            Logger.WriteLine("\n\n");
        }
    }
}
