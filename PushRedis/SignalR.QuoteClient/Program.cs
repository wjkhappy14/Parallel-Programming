using System;
using System.Diagnostics;
using System.Linq;
using Microsoft.Extensions.CommandLineUtils;

namespace SignalR.QuoteClient
{
    public class Program
    {
        public static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                args = new string[1];
                args[0] = "dynamic";//streaming chat upload hubT talk  dynamic
            }

            if (args.Contains("--debug"))
            {
                Console.WriteLine($"Ready for debugger to attach. Process ID: {Process.GetCurrentProcess().Id}");
                Console.Write("Press ENTER to Continue");
                Console.ReadLine();
                args = args.Except(new[] { "--debug" }).ToArray();
            }

            var app = new CommandLineApplication();
            app.FullName = "SignalR Client Samples";
            app.Description = "Client  for SignalR";

            MessagesConnectionHandler.Register(app);//chat
            HubSample.Register(app);//hubT
            TalkSample.Register(app);//talk
            DynamicSample.Register(app);//dynamic
            StreamingSample.Register(app);//streaming
            UploadSample.Register(app);//upload

            app.Command("help", cmd =>
            {
                cmd.Description = "Get help for the application, or a specific command";

                var commandArgument = cmd.Argument("<COMMAND>", "The command to get help for");
                cmd.OnExecute(() =>
                {
                    app.ShowHelp(commandArgument.Value);
                    return 0;
                });
            });

            app.OnExecute(() =>
            {
                app.ShowHelp();
                return 0;
            });

            app.Execute(args);
        }
    }
}
