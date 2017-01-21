@if-sdf = 0
	sdf 7, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 1, -1
	ex2 -1, -1, -1
	goto -1
@rect-change-ter = 1
	sdf 84, 100
	msg -1, -1, -1
	pic 0, 4
	ex1 55, 8, -1
	ex2 55, 10, -1
	goto -1
@set-sector = 2
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 0, 2, -1
	ex2 12, 3, -1
	goto 3
@if-sdf = 3
	sdf 7, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 4, -1
	ex2 -1, -1, -1
	goto 6
@once-set-sdf = 4
	sdf 9, 0
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 5
@disp-msg = 5
	sdf -1, -1
	msg 0, 1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-set-sdf = 6
	sdf 9, 1
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 7
@disp-msg = 7
	sdf -1, -1
	msg 2, 3, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@set-sector = 8
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 0, -1
	ex2 15, 29, -1
	goto 11
@button-generic = 9
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 14, 43, -1
	ex2 7, 2, -1
	goto -1
@set-sector = 10
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 2, 0, -1
	ex2 15, 29, -1
	goto 9
@button-generic = 11
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 24, 45, -1
	ex2 8, 2, -1
	goto -1
@once = 12
	sdf 9, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 13
@once-dlog = 13
	sdf -1, -1
	msg 4, -1, 1
	pic 137, 1
	ex1 37, 14, -1
	ex2 -1, -1, -1
	goto -1
@inc-sdf = 14
	sdf 9, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 0, -1
	ex2 -1, -1, -1
	goto 15
@hp = 15
	sdf -1, -1
	msg 10, -1, -1
	pic 0, 4
	ex1 10, 0, -1
	ex2 -1, -1, -1
	goto 16
@if-sdf = 16
	sdf 9, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 5, 17, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 17
	sdf -1, -1
	msg 11, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto 18
@once-set-sdf = 18
	sdf 9, 2
	msg -1, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 19
	sdf 9, 6
	msg 12, -1, 1
	pic 95, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-statistic = 20
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 10, 22, -1
	ex2 11, 0, -1
	goto 21
@disp-msg = 21
	sdf -1, -1
	msg 18, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@spell-mage = 22
	sdf -1, -1
	msg 19, -1, -1
	pic 0, 4
	ex1 36, 0, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 23
	sdf -1, -1
	msg 20, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 24
	sdf 9, 7
	msg 21, -1, 1
	pic 102, 3
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-disp-msg = 25
	sdf 9, 8
	msg 27, 28, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 26
	sdf 9, 9
	msg 29, -1, 8
	pic 76, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-trap = 27
	sdf 100, 1
	msg -1, -1, -1
	pic 27, 4
	ex1 0, 1, -1
	ex2 10, -1, -1
	goto -1
@once-disp-msg = 28
	sdf 100, 2
	msg 35, 36, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 29
	sdf 9, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 30, -1
	ex2 -1, -1, -1
	goto -1
@once-give-dlog = 30
	sdf 9, 4
	msg 37, -1, 2
	pic 228, 1
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@once-dlog = 31
	sdf 100, 3
	msg 43, -1, 1
	pic 18, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 32
	sdf -1, -1
	msg 49, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-statistic = 33
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 8, 35, -1
	ex2 11, 0, -1
	goto 34
@disp-msg = 34
	sdf -1, -1
	msg 50, 51, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@spell-mage = 35
	sdf -1, -1
	msg 52, 53, -1
	pic 0, 4
	ex1 32, 0, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 36
	sdf 9, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 37, -1
	ex2 -1, -1, -1
	goto -1
@disp-msg = 37
	sdf -1, -1
	msg 54, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 38
	sdf 9, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 39, -1
	ex2 -1, -1, -1
	goto 40
@disp-msg = 39
	sdf -1, -1
	msg 55, -1, -1
	pic 0, 4
	ex1 -1, -1, -1
	ex2 -1, -1, -1
	goto -1
@if-sdf = 40
	sdf 100, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, 42, -1
	ex2 -1, -1, -1
	goto 41
@set-sdf = 41
	sdf 100, 5
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto 45
@if-spec-item = 42
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
	ex1 8, 43, -1
	ex2 -1, -1, -1
	goto -1
@once-give-spec-item = 43
	sdf -1, -1
	msg 56, 57, -1
	pic 0, 4
	ex1 8, 1, -1
	ex2 -1, -1, -1
	goto 44
@set-sdf = 44
	sdf 9, 3
	msg -1, -1, -1
	pic 0, 4
	ex1 1, -1, -1
	ex2 -1, -1, -1
	goto -1
@nop = 45
	sdf -1, -1
	msg -1, -1, -1
	pic 0, 4
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
