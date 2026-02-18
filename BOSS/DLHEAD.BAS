on error goto 10
open "temp" for input as #1
open "dlload.txt" for output as #2

print #2,""
print #2,chr$(1)+"11                        BOSS DOWNLOAD LOG ["+date$+"]"
print #2,chr$(1)+"12                        =============================="
print #2,""
print #2,chr$(1)+"10"

while not eof(1)

line input #1,a$
print #2,"                       "+a$

wend

print #2,""
print #2,chr$(3)
close #1,2

10 system
