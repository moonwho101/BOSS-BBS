
for i=0 to 99

x$=str$(i)





f$="menu"+mid$(x$,2,len(x$))+"-2"+".ans"

r$="menu"+mid$(x$,2,len(x$))+"-2"+".asc"

shell "copy "+f$+" "+r$

next i


