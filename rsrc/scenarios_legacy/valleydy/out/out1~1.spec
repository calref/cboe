@block-move = 0
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 1, -1, -1
	goto -1
@if-sdf = 1
	sdf 204, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 2, -1
	ex2 -1, -1, -1
	goto 3
@disp-msg = 2
	sdf -1, -1
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 3
	sdf 204, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 4, -1
	ex2 -1, -1, -1
	goto 5
@disp-msg = 4
	sdf -1, -1
	msg 1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-spec-item = 5
	sdf -1, -1
	msg -1, 1, -1
	pic 0, 4
	ex1 7, 22, -1
	ex2 -1, -1, -1
	goto 9
@once-dlog = 6
	sdf -1, -1
	msg 2, -1, 1
	pic 73, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 7
@spell-mage = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 32, 0, -1
	ex2 -1, -1, -1
	goto 8
@spell-mage = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 31, 0, -1
	ex2 -1, -1, -1
	goto 23
@if-sdf = 9
	sdf 204, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 10, -1
	ex2 -1, -1, -1
	goto 11
@disp-msg = 10
	sdf -1, -1
	msg 8, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 11
	sdf -1, -1
	msg 9, -1, 1
	pic 73, 3
	ex1 22, 13, -1
	ex2 29, 15, -1
	goto 12
@block-move = 12
	sdf -1, -1
	msg 15, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto 18
@set-sdf = 13
	sdf 204, 0
	msg 16, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 14
@make-out-monst = 14
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 -1, -1, -1
	goto 20
@block-move = 15
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 0, -1, -1
	goto 16
@once-dlog = 16
	sdf -1, -1
	msg 17, -1, 1
	pic 73, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 17
@set-sdf = 17
	sdf 204, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 19
	sdf -1, -1
	msg 23, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 21
	sdf -1, -1
	msg 24, -1, 1
	pic 85, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 22
	sdf 204, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 6, -1
	ex2 -1, -1, -1
	goto 11
@set-sdf = 23
	sdf 204, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
