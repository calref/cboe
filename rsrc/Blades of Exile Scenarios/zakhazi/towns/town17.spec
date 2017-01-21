@if-ter = 0
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 35, 29, -1
	ex2 148, 1, -1
	goto 4
@change-ter = 1
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 35, 29, -1
	ex2 147, -1, -1
	goto 2
@change-ter = 2
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 36, 29, -1
	ex2 148, -1, -1
	goto 3
@disp-sm-msg = 3
	sdf -1, -1
	msg 0, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-sm-msg = 4
	sdf -1, -1
	msg 1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 5
@rect-change-ter = 5
	sdf 147, 100
	msg -1, -1, -1
	pic 0, 4
	ex1 29, 35, -1
	ex2 29, 39, -1
	goto 6
@change-ter = 6
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 35, 29, -1
	ex2 148, -1, -1
	goto -1
@if-ter = 7
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 36, 29, -1
	ex2 148, 8, -1
	goto 4
@change-ter = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 36, 29, -1
	ex2 147, -1, -1
	goto 9
@change-ter = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 37, 29, -1
	ex2 148, -1, -1
	goto 3
@if-ter = 10
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 37, 29, -1
	ex2 148, 11, -1
	goto 4
@change-ter = 11
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 37, 29, -1
	ex2 147, -1, -1
	goto 12
@change-ter = 12
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 38, 29, -1
	ex2 148, -1, -1
	goto 3
@if-ter = 13
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 38, 29, -1
	ex2 148, 14, -1
	goto 4
@change-ter = 14
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 38, 29, -1
	ex2 147, -1, -1
	goto 15
@change-ter = 15
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 39, 29, -1
	ex2 148, -1, -1
	goto 3
@disp-msg = 16
	sdf -1, -1
	msg 2, 3, -1
	pic 0, 4
	ex1 0, 1, -1
	ex2 -1, -1, -1
	goto 17
@death = 17
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 18
	sdf 17, 0
	msg 4, -1, 1
	pic 92, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 19
	sdf 17, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 20, -1
	ex2 -1, -1, -1
	goto 21
@disp-msg = 20
	sdf -1, -1
	msg 10, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 21
	sdf -1, -1
	msg 11, -1, 1
	pic 22, 4
	ex1 8, 23, -1
	ex2 -1, -1, -1
	goto 22
@block-move = 22
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@split-party = 23
	sdf -1, -1
	msg 17, -1, -1
	pic 0, 4
	ex1 25, 42, -1
	ex2 10, -1, -1
	goto -1
@nop = 24
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 25
	sdf 17, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 26, -1
	ex2 -1, -1, -1
	goto 27
@block-move = 26
	sdf -1, -1
	msg 18, 19, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-dlog = 27
	sdf -1, -1
	msg 20, -1, 1
	pic 22, 4
	ex1 8, 29, -1
	ex2 -1, -1, -1
	goto 28
@block-move = 28
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@unite-party = 29
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, -1, -1
	ex2 0, -1, -1
	goto -1
@disp-msg = 30
	sdf -1, -1
	msg 26, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 31
	sdf -1, -1
	msg 27, -1, 1
	pic 168, 1
	ex1 34, 32, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 32
	sdf -1, -1
	msg 33, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 33
	sdf -1, -1
	msg 27, -1, 1
	pic 168, 1
	ex1 34, 34, -1
	ex2 -1, -1, -1
	goto -1
@if-statistic = 34
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 18, 36, -1
	ex2 11, 0, -1
	goto 35
@disp-msg = 35
	sdf -1, -1
	msg 34, 35, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@spell-mage = 36
	sdf -1, -1
	msg 36, 37, -1
	pic 0, 4
	ex1 60, 0, -1
	ex2 -1, -1, -1
	goto -1
@block-move = 37
	sdf -1, -1
	msg 38, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@once-disp-msg = 38
	sdf 17, 4
	msg 39, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 39
	sdf 17, 5
	msg 40, 41, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-town-encounter = 40
	sdf 17, 6
	msg 42, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 41
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-ter = 42
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 40, 5, -1
	ex2 147, 43, -1
	goto -1
@change-ter = 43
	sdf -1, -1
	msg 43, -1, -1
	pic 0, 4
	ex1 40, 5, -1
	ex2 148, -1, -1
	goto -1
@if-ter = 44
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 40, 5, -1
	ex2 148, 45, -1
	goto -1
@change-ter = 45
	sdf -1, -1
	msg 44, -1, -1
	pic 0, 4
	ex1 40, 5, -1
	ex2 147, -1, -1
	goto -1
@once-give-dlog = 46
	sdf 17, 3
	msg 45, -1, 1
	pic 167, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 47
	sdf -1, -1
	msg 51, -1, 1
	pic 22, 4
	ex1 8, 48, -1
	ex2 -1, -1, -1
	goto -1
@unite-party = 48
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, -1, -1
	ex2 0, -1, -1
	goto -1
@if-sdf-eq = 49
	sdf 17, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 50, -1
	ex2 -1, -1, -1
	goto 51
@set-sdf = 50
	sdf 17, 1
	msg 57, 58, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 55
@if-sdf-eq = 51
	sdf 17, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 53, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 52
	sdf -1, -1
	msg 59, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 53
	sdf 17, 7
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 54, -1
	ex2 -1, -1, -1
	goto 52
@set-sdf = 54
	sdf 17, 2
	msg 60, 61, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@town-visible = 55
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 19, 1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 56
	sdf 17, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 58, -1
	ex2 -1, -1, -1
	goto 57
@block-move = 57
	sdf -1, -1
	msg 62, -1, -1
	pic 0, 4
	ex1 0, -1, -1
	ex2 0, -1, -1
	goto -1
@portal-generic = 58
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 2, -1
	ex2 -1, -1, -1
	goto -1
@town-visible = 59
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 19, 1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 60
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 61
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 62
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 63
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 64
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 65
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 66
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 67
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 68
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 69
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 70
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 71
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 72
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 73
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 74
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 75
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 76
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 77
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 78
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 79
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 80
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 81
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 82
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 83
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 84
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 85
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 86
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 87
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 88
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 89
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 90
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 91
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 92
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 93
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 94
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 95
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 96
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 97
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 98
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 99
	sdf -1, -1
	msg -1, -1, -1
	pic -1, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
