'''
sys.argv[1] should be PID of the followed process.
sys.argv[2] should be output file name. The script automatically
adds '.scv' at the end, so don't put any extension.

'''
import sys


kernelLog = open('/var/log/kern.log', 'r')
scvFile = open(sys.argv[2]+'.csv', 'w')
shifts = ''
pid = (sys.argv[1])

for line in kernelLog:
	i = line.find('[%s]: execution has begun. Locality size is ' % pid)
	if i >= 0:
                name = line[:i]
                name = name[name.find(' ')+1:]
                size = len('[%s]: execution has begun. Locality size is ' % pid)
                scvFile.write(('%s[%s]'%(name,pid)) +', locality size %s.\nFaults graph:\n ' % line[i+size:]+'\n')
        i = line.find('[%s]: execution has ended' % pid)
        if i >= 0:
                break
        i = line.find('[%s] tick: ' % pid)
        if i >= 0:
                size = len('[%s] tick: ' % pid)
                scvFile.write(line[i+size:]+'\n')
        i = line.find('[%s] shift: ' % pid)
        if i >= 0:
                size = len('[%s] shift: ' % pid)
                shifts += ('\n' + line[i+size:])
                              

scvFile.write('###################Phase shifts graph###################' + shifts)
kernelLog.close()
scvFile.close()
