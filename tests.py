import pipes
import names
import time

import pipes
t = pipes.Template()
t.append('tr a-z A-Z', '--')
f = t.open('np_client_server', 'w')
time.sleep(1)
f.write('Joana 10 10 1')

time.sleep(100)
f.close()
