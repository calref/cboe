@if-sdf = 0
	sdf 100, 8
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 2, -1
	ex2 -1, -1, -1
	goto 1
@disp-msg = 1
	sdf -1, -1
	msg 0, 1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 2
	sdf 101, 1
	msg -1, 1, -1
	pic 0, 4
	ex1 1, 3, -1
	ex2 -1, -1, -1
	goto 4
@disp-msg = 3
	sdf -1, -1
	msg 2, 3, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 4
	sdf -1, -1
	msg 4, -1, 1
	pic 66, 1
	ex1 41, 14, -1
	ex2 -1, -1, -1
	goto -1
@out-move-party = 5
	sdf -1, -1
	msg 10, -1, -1
	pic 0, 4
	ex1 33, 20, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 6
	sdf 101, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 7, -1
	ex2 -1, -1, -1
	goto 10
@once-dlog = 7
	sdf -1, -1
	msg 11, -1, 0
	pic 19, 3
	ex1 2, 9, -1
	ex2 3, 8, -1
	goto -1
@death = 8
	sdf -1, -1
	msg 17, 18, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto -1
@out-move-party = 9
	sdf -1, -1
	msg 19, 20, -1
	pic 0, 4
	ex1 44, 21, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 10
	sdf -1, -1
	msg 21, -1, 1
	pic 64, 1
	ex1 41, 11, -1
	ex2 -1, -1, -1
	goto -1
@if-gold = 11
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 13, -1
	ex2 1, -1, -1
	goto 12
@block-move = 12
	sdf -1, -1
	msg 27, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto -1
@out-move-party = 13
	sdf -1, -1
	msg 28, -1, -1
	pic 0, 4
	ex1 39, 25, -1
	ex2 -1, -1, -1
	goto -1
@if-gold = 14
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 5, -1
	ex2 1, -1, -1
	goto 12
@block-move = 15
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@once-dlog = 16
	sdf 200, 0
	msg 29, -1, 1
	pic 10, 3
	ex1 41, 18, -1
	ex2 -1, -1, -1
	goto 17
@disp-msg = 17
	sdf -1, -1
	msg 35, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-gold = 18
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 500, 19, -1
	ex2 1, -1, -1
	goto 20
@once-give-item = 19
	sdf -1, -1
	msg 36, -1, -1
	pic 0, 4
	ex1 285, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 20
	sdf -1, -1
	msg 37, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-out-encounter = 21
	sdf 200, 1
	msg 38, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 22
	sdf 101, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 23, -1
	ex2 -1, -1, -1
	goto 24
@disp-msg = 23
	sdf -1, -1
	msg 39, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 24
	sdf -1, -1
	msg 40, -1, 1
	pic 190, 1
	ex1 7, 25, -1
	ex2 -1, -1, -1
	goto -1
@start-shop = 25
	sdf -1, -1
	msg 46, -1, -1
	pic 0, 4
	ex1 31, 1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 26
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 27
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 28
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 29
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
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
