OPEN "header.bbs" FOR RANDOM AS #3 LEN = 125
FIELD #3, 5 AS msg$, 2 AS ar$, 1 AS sta$, 1 AS reci$, 8 AS dat$, 11 AS tyme$, 19 AS to1$, 19 AS to2$, 19 AS from$, 35 AS sj$, 5 AS tra$
10
print "Enter Message #: ";:input a$



get #3,val(a$)

print to1$,from$

print ar$

lset ar$="15"
lset sta$="N"
put #3,val(a$)

close #3


