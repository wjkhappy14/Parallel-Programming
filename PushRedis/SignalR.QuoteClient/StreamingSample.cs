using System;
using System.Threading.Tasks;
using Microsoft.AspNetCore.SignalR.Client;
using Microsoft.Extensions.CommandLineUtils;

namespace SignalR.QuoteClient
{


    /// <summary>
    /// http://localhost:5000/streaming
    /// </summary>
    internal class StreamingSample
    {
        internal static void Register(CommandLineApplication app)
        {
            app.Command("streaming", cmd =>
            {
                cmd.Description = "Tests a streaming connection to a hub";

                var baseUrlArgument = cmd.Argument("<BASEURL>", "The URL to the Chat Hub to test");

                cmd.OnExecute(() => ExecuteAsync(baseUrlArgument.Value));
            });
        }

        public static async Task<int> ExecuteAsync(string baseUrl)
        {
            baseUrl = string.IsNullOrEmpty(baseUrl) ? "http://localhost:5000/streaming" : baseUrl;
            var connection = new HubConnectionBuilder()
                .WithUrl(baseUrl)
                .Build();

            await connection.StartAsync();

            var reader = await connection.StreamAsChannelAsync<int>("ChannelCounter", 10, 2000);

            while (await reader.WaitToReadAsync())
            {
                while (reader.TryRead(out var item))
                {
                    Console.WriteLine($"received: {item}");
                }
            }

            return 0;
        }
    }
}
