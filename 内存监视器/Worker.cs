using Microsoft.Win32;

namespace 内存监视器
{
    public class Worker : BackgroundService
    {
        private readonly ILogger<Worker> _logger;
		static readonly RegistryKey 注册表键 = Registry.LocalMachine.CreateSubKey("SOFTWARE\\埃博拉酱\\内存监视器", true);
		static readonly EventId 事件ID = new(2);
		readonly Timer 定时器;
		async Task 定时检查()
		{
			try
			{
				
			}
			catch (Exception ex)
			{
				_logger.LogError(事件ID, ex, null);
			}
		}
		public Worker(ILogger<Worker> logger)
        {
            _logger = logger;
			定时器 = new Timer((state) => _ = 定时检查());
		}

        protected override Task ExecuteAsync(CancellationToken stoppingToken)
		{
			return 定时检查();
		}
    }
}
