@once-disp-msg = 0
	sdf 3, 0
	msg 0, 1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 1
@once-town-encounter = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 2
	sdf 3, 1
	msg 2, 3, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 3
	sdf -1, -1
	msg 4, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 4
	sdf -1, -1
	msg 5, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 5
	sdf -1, -1
	msg 6, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 6
	sdf -1, -1
	msg 7, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 7
	sdf -1, -1
	msg 8, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 8
	sdf -1, -1
	msg 9, -1, 1
	pic 154, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 9
	sdf 3, 2
	msg 15, 16, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 10
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 5, 11, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 11
	sdf 3, 3
	msg 17, 18, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-trap = 12
	sdf 3, 4
	msg 19, -1, -1
	pic 27, 4
	ex1 2, 3, -1
	ex2 75, -1, -1
	goto -1
@disp-msg = 13
	sdf -1, -1
	msg 20, 21, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-ter = 14
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 17, 44, -1
	ex2 163, 15, -1
	goto -1
@change-ter = 15
	sdf -1, -1
	msg 22, -1, -1
	pic 0, 4
	ex1 17, 44, -1
	ex2 162, -1, -1
	goto 16
@change-ter = 16
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 18, 44, -1
	ex2 162, -1, -1
	goto -1
@once-dlog = 17
	sdf -1, -1
	msg 23, -1, 1
	pic 9, 4
	ex1 35, 18, -1
	ex2 -1, -1, -1
	goto -1
@if-ter = 18
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 19, 55, -1
	ex2 162, 20, -1
	goto 19
@place-monst = 19
	sdf -1, -1
	msg 29, -1, -1
	pic 0, 4
	ex1 15, 56, -1
	ex2 188, 1, -1
	goto -1
@change-ter = 20
	sdf -1, -1
	msg 30, -1, -1
	pic 0, 4
	ex1 19, 56, -1
	ex2 163, -1, -1
	goto 21
@change-ter = 21
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 19, 55, -1
	ex2 163, -1, -1
	goto 38
@if-ter = 22
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 29, 55, -1
	ex2 162, 23, -1
	goto -1
@change-ter = 23
	sdf -1, -1
	msg 31, -1, -1
	pic 0, 4
	ex1 29, 55, -1
	ex2 163, -1, -1
	goto 24
@change-ter = 24
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 29, 56, -1
	ex2 163, -1, -1
	goto -1
@if-ter = 25
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 41, 55, -1
	ex2 162, 26, -1
	goto -1
@change-ter = 26
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 41, 55, -1
	ex2 163, -1, -1
	goto 27
@change-ter = 27
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 41, 56, -1
	ex2 163, -1, -1
	goto 28
@rect-place-field = 28
	sdf 100, 5
	msg 32, 33, -1
	pic 0, 4
	ex1 53, 32, -1
	ex2 60, 40, -1
	goto -1
@if-ter = 29
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 41, 55, -1
	ex2 163, 30, -1
	goto -1
@rect-place-field = 30
	sdf 100, 6
	msg 34, -1, -1
	pic 0, 4
	ex1 53, 32, -1
	ex2 60, 40, -1
	goto -1
@if-ter = 31
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 55, 50, -1
	ex2 162, 32, -1
	goto 33
@once-dlog = 32
	sdf -1, -1
	msg 36, -1, 1
	pic 160, 1
	ex1 36, 34, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 33
	sdf -1, -1
	msg 35, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@select-pc = 34
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto 35
@damage = 35
	sdf -1, -1
	msg 42, 43, -1
	pic 0, 4
	ex1 10, 10, -1
	ex2 10, 1, -1
	goto 36
@rect-change-ter = 36
	sdf 163, 100
	msg -1, -1, -1
	pic 0, 4
	ex1 50, 55, -1
	ex2 50, 56, -1
	goto 39
@disp-msg = 37
	sdf -1, -1
	msg 44, 45, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@place-monst = 38
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 15, 56, -1
	ex2 188, 1, -1
	goto -1
@change-ter = 39
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 51, 58, -1
	ex2 210, -1, -1
	goto 40
@change-ter = 40
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 53, 58, -1
	ex2 214, -1, -1
	goto -1
@button-generic = 41
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 9, -1
	ex2 4, 3, -1
	goto -1
@button-generic = 42
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 40, 30, -1
	ex2 4, 3, -1
	goto -1
