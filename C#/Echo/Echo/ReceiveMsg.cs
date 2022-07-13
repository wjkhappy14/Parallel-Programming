namespace Echo
{
    public class ReceiveMsg
    {
        private MsgAwaitableEventArgs Msg { get; set; }
        public ReceiveMsg()
        {
            Msg = new MsgAwaitableEventArgs();
        }

        public MsgAwaitableEventArgs WaitForDataAsync()
        {
            return Msg;
        }

    }
}
