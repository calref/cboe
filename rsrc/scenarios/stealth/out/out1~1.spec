@once-dlog = 0
	sdf 205, 0
	msg 0, -1, 1
	pic 85, 1
	ex1 8, 1, -1
	ex2 -1, -1, -1
	goto 2
@once-dlog = 1
	sdf -1, -1
	msg 6, -1, 1
	pic 17, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 2
	sdf -1, -1
	msg 12, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@if-sdf = 3
	sdf 100, 8
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 6, -1
	ex2 -1, -1, -1
	goto 4
@once-dlog = 4
	sdf -1, -1
	msg 13, -1, 1
	pic 16, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 5
@block-move = 5
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 6
	sdf -1, -1
	msg 19, -1, 1
	pic 16, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@once-disp-msg = 8
	sdf 205, 1
	msg 25, 26, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto 10
@once-dlog = 10
	sdf -1, -1
	msg 27, -1, 1
	pic 41, 3
	ex1 26, 12, -1
	ex2 -1, -1, -1
	goto 11
@disp-msg = 11
	sdf -1, -1
	msg 33, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@damage = 12
	sdf -1, -1
	msg 34, -1, -1
	pic 0, 4
	ex1 8, 10, -1
	ex2 1, 0, -1
	goto -1
@once = 13
	sdf 205, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 9
@once-set-sdf = 14
	sdf 205, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 15
	sdf 205, 3
	msg 35, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 16
	sdf -1, -1
	msg 36, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 17
	sdf 205, 4
	msg 37, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-item = 18
	sdf -1, -1
	msg 38, 39, -1
	pic 0, 4
	ex1 203, 1000, -1
	ex2 -1, -1, -1
	goto 19
@once-give-item = 19
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 248, -1, -1
	ex2 -1, -1, -1
	goto 20
@once-give-item = 20
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 322, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 21
	sdf -1, -1
	msg 40, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 22
	sdf 205, 5
	msg 41, -1, 1
	pic 168, 3
	ex1 22, 24, -1
	ex2 -1, -1, -1
	goto 23
@block-move = 23
	sdf -1, -1
	msg 47, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@status = 24
	sdf -1, -1
	msg 48, -1, -1
	pic 0, 4
	ex1 8, 1, 3
	ex2 -1, -1, -1
	goto 25
@once-out-encounter = 25
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 26
	sdf 205, 6
	msg 49, -1, -1
	pic 0, 4
	ex1 3, -1, -1
	ex2 -1, -1, -1
	goto -1
@gold = 27
	sdf -1, -1
	msg 50, -1, -1
	pic 0, 4
	ex1 500, 0, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 28
	sdf 205, 7
	msg 51, -1, -1
	pic 2, 4
	ex1 365, -1, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 29
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@nop = 30
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 31
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 32
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 33
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 34
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 35
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 36
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 37
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 38
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 39
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 40
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 41
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 42
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 43
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 44
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 45
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 46
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 47
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 48
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 49
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 50
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 51
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 52
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 53
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 54
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 55
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 56
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 57
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 58
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 59
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
