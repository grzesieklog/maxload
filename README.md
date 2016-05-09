#About maxload
In intention it is a Unix shell command which run other task, like nice command. nice tools is static, just run another process with some nice level. maxload goes much further, dynamical change nice level (with the IO priority) depending for system load level, even pause controled process if it necessary. So maxload is excellent tool to run CPU intensive task as a system overload protection.

#Introduction
Some of task are very resource intensive, on the other hand system load is changing over time. The best practice is running intensive task in time when system load is be low. The maxload tool trying solve this problem by continuous monitoring system load and dynamically controls process priority. In example, if you do backups on your server in the night when usually system is not busy the task will by run normaly. But when some of service will begin working and generate heigth system load the maxload will release resources used by his controlled process. When system load will drop to low level, the controlled process will by starts again, even maxload can restore the orginal priority of controlled process if it's possible (run as root).

See how it is work on video: https://youtu.be/IwpuLnNWhrY
