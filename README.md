#About `maxload`
In intention it is a Unix shell command which run other task, like `nice` command. `nice` tools is static, just run another process with some nice level. `maxload` goes much further, dynamical change nice level (with the I/O priority) depending of system load level, even pause controled process if it necessary. So `maxload` is excellent tool to run CPU intensive task as a system overload protection.

#Introduction
Some of task are very resource intensive, on the other hand system load is changing over time. The best practice is running intensive task in time when system load is be low. The `maxload` tool trying solve this problem by continuous monitoring system load and dynamically controls process priority. In example, if you do backups on your server in the night when usually system is not busy the task will by run normaly. But when some of service will begin working and generate heigth system load the `maxload` will release resources used by his controlled process (not memory). When system load will drop to low level the controlled process will by starts again, even `maxload` can restore the orginal priority of controlled process if it's possible (run as `root`).

#How to use `maxload`
`maxload` have simple syntax:
```
maxload [load] [command] [args ...]
```
You need two things to run `maxload`, max load border `[load]` and command you want to run `[command]`. If this command has arguments you type it normaly after command. Example:
```
maxload 1.9 ./backup.sh /home
```
In this case `maxload` have border `1.9` and run command `./backup.sh /home`.

##Proper border
The most important things is to set proper border value. This value is depend of the CPU cores number which you have in your system. You can check this by command: `top`, `cat /proc/cpuinfo` or `nproc`.

One CPU core means maximum 1 value of system load, value bigger on 1 means your system is overloads. So if you have two cores the maximum load is 2, for four cores it's 4.

Suppose you have two CPU cores and you have run backup script until system is not overload. If you set border on `2` `maxload` start work when overload is occurs, and users can experiencing the overload. To avoid this you must set border less then maximum load e.g. `1.9`.

##Demonstration video
You can see how `maxload` works on this video [how works maxload](https://youtu.be/IwpuLnNWhrY)

##FAQ
###Which value of system load averages is used in `maxload`: 1, 5, 15 minutes?

5 minutes.

