<h3>About <code>maxload</code></h3>
In intention it is a Unix shell command which run other task, like <code>nice</code> command. <code>nice</code> tools is static, just run another process with some nice level. <code>maxload</code> goes much further, dynamical change nice level (with the I/O priority) depending of system load level, even pause controled process if it necessary. So <code>maxload</code> is excellent tool to run CPU intensive task as a system overload protection.

<h3>Introduction</h3>
Some of task are very resource intensive, on the other hand system load is changing over time. The best practice is running intensive task in time when system load is be low. The <code>maxload</code> tool trying solve this problem by continuous monitoring system load and dynamically controls process priority. In example, if you do backups on your server in the night when usually system is not busy the task will by run normaly. But when some of service will begin working and generate heigth system load the <code>maxload</code> will release resources used by his controlled process (not memory). When system load will drop to low level the controlled process will by starts again, even <code>maxload</code> can restore the normal priority for controlled process if it's possible (run as <code>root</code>).

<h3>How to use <code>maxload</code></h3>
<code>maxload</code> has simple syntax:
<code>
maxload [load] [command] [args ...]
</code>
You need two things to run <code>maxload</code>, max load border <code>[load]</code> and command you want to run <code>[command]</code>. If this command has arguments you type it normaly after command. Example:
<code>
maxload 1.9 ./backup.sh /home
</code>
In this case <code>maxload</code> has border <code>1.9</code> and run command <code>./backup.sh /home</code>.

<h3>Proper border</h3>
The most important things is to set proper border value. This value is depend of the CPU cores number which you have in your system. You can check this by command: <code>top</code>, <code>cat /proc/cpuinfo</code> or <code>nproc</code>.

One CPU core means maximum 1 value of system load, value bigger then 1 means your system is overloads. So if you have two cores the maximum load is 2, for four cores it's 4.

Suppose you have two CPU cores and you have run backup script until system is not overload. If you set border on <code>2</code> <code>maxload</code> start work when overload is occurs, and users can experiencing the overload. To avoid this you must set border less then maximum load e.g. <code>1.9</code>.

<h3>Strategy of maxload</h3>
<code>maxload</code> changing priority for controlled process during its executing. This change has impact to process scheduling in operating system so it shouldn't happen frequently. Linux kernel calculates load averages from every 5 seconds. So 5 seconds is the highest frequency with <code>maxload</code> work.

Here is the list of <code>maxload</code> frequency work in relative to max border:
- load <= 10% border (checks every 15 seconds)
- 11% border <= load <= 25% border (checks every 10 seconds)
- load > 25% border (checks every 5 seconds)

<code>maxload</code> starts reduce priority for controlled process if system load is between 40-100% of set border and grows.

<code>maxload</code> has four steps to get the lowest priority value for controled process.
Here is the list how <code>maxload</code> changes priority for controlled process:
- LEVEL1: CPU nice 5, I/O priority 4
- LEVEL2: CPU nice 10, I/O priority 5
- LEVEL3: CPU nice 15, I/O priority 6
- LEVEL4: CPU nice 19, I/O priority 7

I/O value is given in best effort (be) scheduler.

If system load exceeds more than set border then <code>maxload</code> paused running controlled process. If system load will by reducing less than set border then <code>maxload</code> starts executing controlled process again.

<h3>Back to normal priority</h3>
<code>maxload</code> is not only decrease priority but can also increase it when system load is low. This is not main goal of <code>maxload</code> but <code>maxload</code> has it. The point is that the <code>maxload</code> is not check what priority it has received from system when it has start executing.
Example, when you run that command as <code>root</code>:

<code># nice -n 9 maxload 3.5 /root/script.sh</code>

and the next system load is growths to <code>3.4</code> and drops back to <code>0.1</code>, in effect the maxload increase priority (value of nice) equal to <code>0</code>.

If <code>maxload</code> in this example run as normal user priority of script will by decreasing to <code>19</code> and not growing even to system load is drops to <code>0.0</code>.

<code>maxload</code> increase priority of controled process if system load is between 75-40% of set border and still decreasing. This case required to <code>maxload</code> must by executes as superuser (<code>root</code>).

<h3>Negative impact of pause executing process</h3>
Pauses in executing process can make the process will not work correctly. Daemon is not good task for running via <code>maxload</code>. Generally linear tasks is good to run via <code>maxload</code>. Good example is the scripts which you run via <code>cron</code>.

<h3>Demonstration video</h3>
You can see how <code>maxload</code> works on this video: [How works maxload](https://youtu.be/IwpuLnNWhrY)

<h3>FAQ</h3>
<ol>
<li>Which value of system load averages is used in <code>maxload</code>: 1, 5, 15 minutes?</li>
1 minutes.
<li>How <code>maxload</code> pauses and restarts controlled process?</li>
Sends <code>SIGSTOP</code> signal to pause and <code>SIGCONT</code> to restore execution.
<li>How <code>maxload</code> execute controlled process?</li>
By <code>fork()</code> function.
</ol>
