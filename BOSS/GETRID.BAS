OPEN "header.bbs" FOR RANDOM AS #3 LEN = 125
FIELD #3, 5 AS msg$, 2 AS ar$, 1 AS sta$, 1 AS reci$, 8 AS dat$, 11 AS tyme$, 19 AS to1$, 19 AS to2$, 19 AS from$, 35 AS sj$, 5 AS tra$
10


for i=1 to 2996




get #3,i

print to1$,from$
if instr(to1$,"Beth") or instr(from$,"Beth S") then lset ar$="D":put #3,i:play "e64"




'lset ar$="15"

'put #3,val(a$)

next i

close #3

reset
