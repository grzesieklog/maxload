#About `maxload`
In intention it is a Unix shell command which run other task, like `nice` command. `nice` tools is static, just run another process with some nice level. `maxload` goes much further, dynamical change nice level (with the I/O priority) depending of system load level, even pause controled process if it necessary. So `maxload` is excellent tool to run CPU intensive task as a system overload protection.

#Introduction
Some of task are very resource intensive, on the other hand system load is changing over time. The best practice is running intensive task in time when system load is be low. The `maxload` tool trying solve this problem by continuous monitoring system load and dynamically controls process priority. In example, if you do backups on your server in the night when usually system is not busy the task will by run normaly. But when some of service will begin working and generate heigth system load the `maxload` will release resources used by his controlled process (not memory). When system load will drop to low level the controlled process will by starts again, even `maxload` can restore the normal priority for controlled process if it's possible (run as `root`).

#How to use `maxload`
`maxload` has simple syntax:
```
maxload [load] [command] [args ...]
```
You need two things to run `maxload`, max load border `[load]` and command you want to run `[command]`. If this command has arguments you type it normaly after command. Example:
```
maxload 1.9 ./backup.sh /home
```
In this case `maxload` has border `1.9` and run command `./backup.sh /home`.

##Proper border
The most important things is to set proper border value. This value is depend of the CPU cores number which you have in your system. You can check this by command: `top`, `cat /proc/cpuinfo` or `nproc`.

One CPU core means maximum 1 value of system load, value bigger then 1 means your system is overloads. So if you have two cores the maximum load is 2, for four cores it's 4.

Suppose you have two CPU cores and you have run backup script until system is not overload. If you set border on `2` `maxload` start work when overload is occurs, and users can experiencing the overload. To avoid this you must set border less then maximum load e.g. `1.9`.

##Strategy of maxload
`maxload` changing priority for controlled process during its executing. This change has impact to process scheduling in operating system so it shouldn't happen frequently. Linux kernel calculates load averages from every 5 seconds. So 5 seconds is the highest frequency with `maxload` work.

Here is the list of `maxload` frequency work in relative to max border:
- load <= 10% border (checks every 15 seconds)
- 11% border <= load <= 25% border (checks every 10 seconds)
- load > 25% border (checks every 5 seconds)

`maxload` starts reduce priority for controlled process if system load is between 40-100% of set border and grows.

`maxload` has four steps to get the lowest priority value for controled process.
Here is the list how `maxload` changes priority for controlled process:
- LEVEL1: CPU nice 5, I/O priority 4
- LEVEL2: CPU nice 10, I/O priority 5
- LEVEL3: CPU nice 15, I/O priority 6
- LEVEL4: CPU nice 19, I/O priority 7

I/O value is given in best effort (be) scheduler.

If system load exceeds more than set border then `maxload` paused running controlled process. If system load will by reducing less than set border then `maxload` starts executing controlled process again.

###Back to normal priority
`maxload` is not only decrease priority but can also increase it when system load is low. This is not main goal of `maxload` but `maxload` has it. The point is that the `maxload` is not check what priority it has received from system when it has start executing.
Example, when you run that command as `root`:
```
# nice -n 9 maxload 3.5 /root/script.sh
```
and the next system load is growths to `3.4` and drops back to `0.1`, in effect the maxload increase priority (value of nice) equal to `0`.

If `maxload` in this example run as normal user priority of script will by decreasing to `19` and not growing even to system load is drops to `0.0`.

`maxload` increase priority of controled process if system load is between 75-40% of set border and still decreasing. This case required to `maxload` must by executes as superuser (`root`).

##Negative impact of pause executing process
Pauses in executing process can make the process will not work correctly. Daemon is not good task for running via `maxload`. Generally linear tasks is good to run via `maxload`. Good example is the scripts which you run via `cron`.

##Demonstration video
You can see how `maxload` works on this video: [How works maxload](https://youtu.be/IwpuLnNWhrY)

##FAQ
####Which value of system load averages is used in `maxload`: 1, 5, 15 minutes?
1 minutes.
####How `maxload` pauses and restarts controlled process?
Sends `SIGSTOP` signal to pause and `SIGCONT` to restore execution.
####How `maxload` execute controlled process?
By `fork()` function.
