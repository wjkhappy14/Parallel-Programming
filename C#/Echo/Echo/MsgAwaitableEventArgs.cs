using System.Runtime.CompilerServices;

namespace Echo
{
    public class MsgAwaitableEventArgs : INotifyCompletion
    {

        //ICriticalNotifyCompletion
        public MsgAwaitableEventArgs GetAwaiter() => this;

        public bool IsCompleted { get; set; }

        public int GetResult() => 20;

        public void OnCompleted(Action continuation)
        {
            continuation();
            IsCompleted = true;
        }

        public void UnsafeOnCompleted(Action continuation)
        {
            continuation();
        }
    }
}
