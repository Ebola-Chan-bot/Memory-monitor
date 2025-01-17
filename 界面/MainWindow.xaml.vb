Class MainWindow
	Private Sub MainWindow_Loaded(sender As Object, e As RoutedEventArgs) Handles Me.Loaded
		Dim 总内存 As ULong = New Devices.ComputerInfo().TotalPhysicalMemory >> 30
		警报内存.Minimum = 总内存 / 5 * 3
		警报内存.Maximum = 总内存
		警报内存.Value = 内核.数据收集器集.警报GB
		强杀内存.Minimum = 总内存 / 5 * 4
		强杀内存.Maximum = 总内存
		强杀内存.Value = 内核.数据收集器集.强杀GB
		Dim 采样间隔时间 As TimeSpan = 内核.数据收集器集.采样间隔
		If 采样间隔时间 < TimeSpan.FromMinutes(1) Then
			采样间隔单位.SelectedValue = "秒"
			采样间隔值.Minimum = 1
			采样间隔值.Maximum = 59
			采样间隔值.Value = 采样间隔时间.TotalSeconds
		ElseIf 采样间隔时间 < TimeSpan.FromHours(1) Then
			采样间隔单位.SelectedValue = "分钟"
			采样间隔值.Minimum = 1
			采样间隔值.Maximum = 59
			采样间隔值.Value = 采样间隔时间.TotalMinutes
		Else
			采样间隔单位.SelectedValue = "小时"
			采样间隔值.Minimum = 1
			采样间隔值.Maximum = 23
			采样间隔值.Value = 采样间隔时间.TotalHours
		End If
	End Sub
End Class
