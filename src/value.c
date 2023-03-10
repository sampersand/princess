#include "value.h"
#include <ctype.h>
#include <string.h>

const array *empty_array = 0;
const integer zero = 0;

array *aalloc(int amnt) {
	array *a = malloc(sizeof(array));
	*a = (array) { 0, amnt, 1, malloc(sizeof(VALUE) * amnt) };
	return a;
}

void afree(array *a) {
	assume(a->rc);
	if (--a->rc) return;
	free(a->items);
	free(a);
}

int eql(VALUE l, VALUE r) {
	if (l == r) return 1;
	if (isint(l) || isint(r)) return 0;

	array *la = ARY(l), *ra = ARY(r);
	if (la->len != ra->len) return 0;

	for (int i = 0; i < la->len; ++i)
		if (!eql(la->items[i], ra->items[i]))
			return 0;
	return 1;
}

integer parse_int(VALUE v) {
	integer i = 0, tmp;
	if (isint(v)) return isdigit(i = INT(v)) ? c2i(i) : 0;

	array *a = ARY(v);
	int idx = 0, sign = 1;

	if (eql(a->items[idx], i2v('-')))
		sign = -1, idx++;

	while (idx < a->len) {
		ensure(isint(a->items[idx]), "can only parse arrays of ints");
		tmp = INT(a->items[idx++]);

		if (!isdigit(tmp)) break;
		i = i*10 + c2i(tmp);
	}

	return i * sign;
}

VALUE duplicate(VALUE v) {
	if (isint(v)) return v;
	array *a = ARY(v);
	array *d = aalloc(a->cap);
	while (d->len < a->len)
		d->items[d->len] = clone(a->items[d->len]), d->len++;
	return a2v(d);
}

void apush(array *a, VALUE v) {
	if (a->len == a->cap) grow(a->items, a->cap);
	a->items[a->len++] = v;
}

VALUE apop(array *a, int n) {
	ensure(n, "indexing starts at 1");
	ensure(n <= a->len, "index %d is out of bounds for stack len %d", n, a->len);
	if (n == 1) return a->items[--a->len];

	VALUE v = a->items[a->len - n];
	if (n == 2) {
		a->items[a->len - 2] = a->items[a->len-1];
		a->len--;
		return v;
	}
	
	do a->items[a->len - n] = a->items[a->len - n - 1]; while (n--);
	a->len--;
	return v;
}

array *to_string(VALUE v) {
	if (!isint(v)) return ARY(clone(v));

	char buf[100]; // large enough
	snprintf(buf, sizeof(buf), "%lld", INT(v));

	array *a = aalloc(strlen(buf));
	for (int i = 0; buf[i]; ++i)
		a->items[a->len++] = i2v(buf[i]);

	return a;
}

void print(VALUE v, FILE *f) {
	if (isint(v))  {
		fputc(INT(v), f);
		return;
	}

	array *a = ARY(v);
	for (int i = 0; i < a->len; ++i) {
		ensure(isint(a->items[i]), "can only print lists of ints");
		fputc(INT(a->items[i]), f);
	}
}

void dump_value(VALUE v, FILE *out) {
	if (isint(v)) {
		fprintf(out, "%lld", INT(v));
		return;
	}

	fputc('[', out);

	const array *a = ARY(v);
	for (int i = 0; i < a->len; ++i) {
		if (i) fputs(", ", out);
		dump_value(a->items[i], out);
	}

	fputc(']', out);
}

VALUE map(VALUE l, VALUE r, integer (*fn)(integer, integer)) {
	if (isint(l) && isint(r))
		return i2v(fn(INT(l), INT(r)));

	die("todo");
	// int ll = len(l), lr = len(r), max = ll < lr ? lr : ll;
	// array *a = aalloc(max);


	// if (isint(l)) {
	// 	for (int i = 0; i < max; ++i) {
	// 		a->items[a->len++] = fn()
	// 	}
	// }

	// for (int i = 0; i < max; ++i) {
	// 	if ()
	// }
}

