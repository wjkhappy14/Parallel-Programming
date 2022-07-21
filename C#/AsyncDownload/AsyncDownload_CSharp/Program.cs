//--------------------------------------------------------------------------
// 
//  Copyright (c) Microsoft Corporation.  All rights reserved. 
// 
//  File: Program.cs
//
//--------------------------------------------------------------------------

using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Threading.Tasks;

class Program
{
    static void Main()
    {
        // NOTE: Synchronous .Wait() calls added only for demo purposes

        string url = "https://www.tallper.cn/notify/wechat/miniapp";



        Parallel.For(0, 1, (index) =>
        {

            UploadStringTask(url, "{}");
            // Single async request
            Download(url).ContinueWith(CompletedDownloadData).Wait();

            // Single async request with timeout
            Download(url).WithTimeout(new TimeSpan(0, 0, 0, 0, 1)).ContinueWith(CompletedDownloadData).Wait();

            // Serial async requests
            Task.Factory.TrackedSequence(
                () => Download(url),
                () => Download(url),
                () => Download(url), // will fail
                () => Download(url),
                () => Download(url)
            ).ContinueWith(SerialTasksCompleted).Wait();

            // Concurrent async requests
            Task.Factory.ContinueWhenAll(new[]
            {
            Download(url),
            Download(url),
            Download(url), // will fail
            Download(url),
            Download(url)
        }, ConcurrentTasksCompleted).Wait();

        });

        // Done
        Console.WriteLine("Press <enter> to exit.");
        Console.ReadLine();
    }

    static Task<byte[]> Download(string url)
    {
        return new WebClient().DownloadDataTask($"{url}?id={Environment.TickCount}");
    }

    static Task<string> UploadStringTask(string url, string data)
    {
        return new WebClient().UploadStringTask($"{url}?id={Environment.TickCount}", "Post", data);
    }

    static void CompletedDownloadData(Task<byte[]> task)
    {
        switch (task.Status)
        {
            case TaskStatus.RanToCompletion:
                Console.WriteLine("Request succeeded: {0}", task.Result.Length);
                break;
            case TaskStatus.Faulted:
                Console.WriteLine("Request failed: {0}", task.Exception.InnerException);
                break;
            case TaskStatus.Canceled:
                Console.WriteLine("Request was canceled");
                break;
        }
    }

    static void SerialTasksCompleted(Task<IList<Task>> tasks)
    {
        int failures = tasks.Result.Where(t => t.Exception != null).Count();
        Console.WriteLine("Serial result: {0} successes and {1} failures", tasks.Result.Count() - failures, failures);
    }

    static void ConcurrentTasksCompleted(Task<byte[]>[] tasks)
    {
        int failures = tasks.Where(t => t.Exception != null).Count();
        Console.WriteLine("Concurrent result: {0} successes and {1} failures", tasks.Length - failures, failures);
    }
}