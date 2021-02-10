// <copyright file="Authentication.cs" company="Microsoft">
// Copyright (c) Microsoft. All rights reserved.
// </copyright>

namespace validatequotes
{
    using System.Collections.Generic;
    using System.Linq;
    using System.Threading.Tasks;
    using Microsoft.Identity.Client;
        public class Authentication
        {
            private const string AcceleratedTokenCacheFileName = "acceleratedcache.bin";
            private const string AuthorityPrefix = "https://login.microsoftonline.com/";
            private const string ClientId = "1950a258-227b-4e31-a9cf-717495945fc2";
            private static readonly string[] Scopes = new[] { "https://attest.azure.net/.default" };

            private static readonly Dictionary<string, string> AcceleratedCache;

            static Authentication()
            {
                AcceleratedCache = SerializationHelper.ReadFromFile<Dictionary<string, string>>(AcceleratedTokenCacheFileName);
            }

            public static async Task<string> AcquireAccessTokenAsync(string tenant, bool forceRefresh)
            {
                string accessToken = null;

                if (!forceRefresh && AcceleratedCache.ContainsKey(tenant))
                {
                    accessToken = AcceleratedCache[tenant];
                }
                else
                {
                    var publicApplication = PublicClientApplicationBuilder.Create(ClientId)
                        .WithAuthority($"{AuthorityPrefix}{tenant}")
                        .WithDefaultRedirectUri()
                        .Build();

                    AuthenticationResult result;
                    try
                    {
                        var accounts = await publicApplication.GetAccountsAsync();
                        result = await publicApplication
                            .AcquireTokenSilent(Scopes, accounts.FirstOrDefault())
                            .ExecuteAsync();
                    }
                    catch (MsalUiRequiredException ex)
                    {
                        result = await publicApplication
                            .AcquireTokenInteractive(Scopes)
                            .WithClaims(ex.Claims)
                            .ExecuteAsync();
                    }

                    accessToken = result.AccessToken;
                    AcceleratedCache[tenant] = accessToken;
                    SerializationHelper.WriteToFile(AcceleratedTokenCacheFileName, AcceleratedCache);
                }

                return accessToken;
            }
        }
 
}
