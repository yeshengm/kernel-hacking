import os, time, curses

def get_info():
	dirs = [s for s in os.listdir('/proc') if s.isdigit()]
	info = []
	for d in dirs:
		pid = comm = ctx = ''
		pid = d
		with open('/proc/' + d + '/comm') as f:
			comm = f.read().strip('\n')
		with open('/proc/' + d + '/ctx') as f:
			ctx = f.read().strip('\n')
		info.append((pid, comm, ctx))
	return info

def info_to_strs():
	top_info = sorted(get_info(), key=lambda i: int(i[2]), reverse=True)[:20]
	strs = []
	for i in top_info:
		strs.append(i[0].rjust(5) + ' ' + i[1].ljust(15) + i[2].rjust(6))
	return strs


screen = curses.initscr()
screen.keypad(1)
screen.nodelay(1)
curses.noecho()

while (True):
	strs = info_to_strs()
	screen.clear()
	for s in strs:
		screen.addstr(s + "\n")
		screen.refresh()
	key = screen.getch()
	if key == ord('q'):
		break;
	time.sleep(1.0)

curses.endwin()
