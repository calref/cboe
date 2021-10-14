@if-sdf = 0
	sdf 101, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 8, -1
	ex2 130, -1, -1
	goto -1
@once-disp-msg = 2
	sdf 2, 0
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 3
	sdf -1, -1
	msg 1, -1, 1
	pic 144, 1
	ex1 51, 4, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 4
	sdf 101, 7
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 5, -1
	ex2 -1, -1, -1
	goto 7
@once-dlog = 5
	sdf -1, -1
	msg 7, -1, 1
	pic 144, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 6
@set-sdf = 6
	sdf 101, 8
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 7
	sdf 101, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 36, -1
	ex2 -1, -1, -1
	goto 10
@once-dlog = 8
	sdf -1, -1
	msg 13, -1, 1
	pic 144, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 9
@set-sdf = 9
	sdf 101, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 10
	sdf 101, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 11, -1
	ex2 -1, -1, -1
	goto 16
@if-sdf = 11
	sdf 100, 9
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 14, -1
	ex2 -1, -1, -1
	goto 12
@once-dlog = 12
	sdf -1, -1
	msg 19, -1, 1
	pic 144, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 13
@set-sdf = 13
	sdf 101, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 14
	sdf -1, -1
	msg 25, -1, 1
	pic 144, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 15
@set-sdf = 15
	sdf 101, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 16
	sdf 100, 8
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 17, -1
	ex2 -1, -1, -1
	goto 20
@if-sdf = 17
	sdf 100, 7
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 18, -1
	ex2 -1, -1, -1
	goto 19
@once-dlog = 18
	sdf -1, -1
	msg 31, -1, 1
	pic 144, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 22
@disp-msg = 19
	sdf -1, -1
	msg 37, 38, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 18
@if-sdf = 20
	sdf 8, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 42, -1
	ex2 -1, -1, -1
	goto 26
@if-sdf = 21
	sdf 100, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 23, -1
	ex2 -1, -1, -1
	goto 25
@set-sdf = 22
	sdf 100, 9
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 23
	sdf -1, -1
	msg 39, -1, 1
	pic 144, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 24
@set-sdf = 24
	sdf 100, 7
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 25
	sdf -1, -1
	msg 45, 46, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 23
@if-sdf = 26
	sdf 100, 4
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 27, -1
	ex2 -1, -1, -1
	goto 29
@once-dlog = 27
	sdf -1, -1
	msg 47, -1, 1
	pic 144, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 28
@set-sdf = 28
	sdf 100, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 29
	sdf -1, -1
	msg 53, -1, 1
	pic 144, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 30
@if-sdf = 30
	sdf 100, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 31, -1
	ex2 -1, -1, -1
	goto 32
@once-dlog = 31
	sdf -1, -1
	msg 59, -1, 1
	pic 144, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 35
@once-dlog = 32
	sdf -1, -1
	msg 67, -1, 0
	pic 144, 1
	ex1 3, 34, -1
	ex2 2, 33, -1
	goto -1
@set-sdf = 33
	sdf 100, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 31
@disp-msg = 34
	sdf -1, -1
	msg 65, 66, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sdf = 35
	sdf 100, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 36
	sdf 100, 9
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 8, -1
	ex2 -1, -1, -1
	goto 12
@if-ter = 37
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 8, -1
	ex2 131, 38, -1
	goto -1
@if-sdf = 38
	sdf 1, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 41, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 39
	sdf -1, -1
	msg 73, -1, -1
	pic 0, 4
	ex1 2, 8, -1
	ex2 130, -1, -1
	goto -1
@button-generic = 40
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 6, 30, -1
	ex2 0, 0, -1
	goto -1
@if-sdf-eq = 41
	sdf 2, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 39, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 42
	sdf 100, 6
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 43, -1
	ex2 -1, -1, -1
	goto 21
@disp-msg = 43
	sdf -1, -1
	msg 74, 75, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 44
	sdf 101, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 45, -1
	ex2 -1, -1, -1
	goto -1
@change-ter = 45
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 8, -1
	ex2 130, -1, -1
	goto -1
