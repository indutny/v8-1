// Copyright 2013 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

(function(global, utils) {

'use strict';

%CheckIsBootstrapping();

// -------------------------------------------------------------------
// Imports

var FLAG_harmony_tolength;
var GetIterator;
var GetMethod;
var GlobalArray = global.Array;
var iteratorSymbol = utils.ImportNow("iterator_symbol");
var MaxSimple;
var MinSimple;
var ObjectIsFrozen;
var ObjectDefineProperty;

utils.Import(function(from) {
  FLAG_harmony_tolength = from.FLAG_harmony_tolength;
  GetIterator = from.GetIterator;
  GetMethod = from.GetMethod;
  MaxSimple = from.MaxSimple;
  MinSimple = from.MinSimple;
  ObjectIsFrozen = from.ObjectIsFrozen;
  ObjectDefineProperty = from.ObjectDefineProperty;
});

// -------------------------------------------------------------------

function InnerArrayCopyWithin(target, start, end, array, length) {
  target = TO_INTEGER(target);
  var to;
  if (target < 0) {
    to = MaxSimple(length + target, 0);
  } else {
    to = MinSimple(target, length);
  }

  start = TO_INTEGER(start);
  var from;
  if (start < 0) {
    from = MaxSimple(length + start, 0);
  } else {
    from = MinSimple(start, length);
  }

  end = IS_UNDEFINED(end) ? length : TO_INTEGER(end);
  var final;
  if (end < 0) {
    final = MaxSimple(length + end, 0);
  } else {
    final = MinSimple(end, length);
  }

  var count = MinSimple(final - from, length - to);
  var direction = 1;
  if (from < to && to < (from + count)) {
    direction = -1;
    from = from + count - 1;
    to = to + count - 1;
  }

  while (count > 0) {
    if (from in array) {
      array[to] = array[from];
    } else {
      delete array[to];
    }
    from = from + direction;
    to = to + direction;
    count--;
  }

  return array;
}

// ES6 draft 03-17-15, section 22.1.3.3
function ArrayCopyWithin(target, start, end) {
  CHECK_OBJECT_COERCIBLE(this, "Array.prototype.copyWithin");

  var array = TO_OBJECT(this);
  var length = TO_LENGTH(array.length);

  return InnerArrayCopyWithin(target, start, end, array, length);
}

function InnerArrayFind(predicate, thisArg, array, length) {
  if (!IS_CALLABLE(predicate)) {
    throw MakeTypeError(kCalledNonCallable, predicate);
  }

  for (var i = 0; i < length; i++) {
    var element = array[i];
    if (%_Call(predicate, thisArg, element, i, array)) {
      return element;
    }
  }

  return;
}

// ES6 draft 07-15-13, section 15.4.3.23
function ArrayFind(predicate, thisArg) {
  CHECK_OBJECT_COERCIBLE(this, "Array.prototype.find");

  var array = TO_OBJECT(this);
  var length = TO_INTEGER(array.length);

  return InnerArrayFind(predicate, thisArg, array, length);
}

function InnerArrayFindIndex(predicate, thisArg, array, length) {
  if (!IS_CALLABLE(predicate)) {
    throw MakeTypeError(kCalledNonCallable, predicate);
  }

  for (var i = 0; i < length; i++) {
    var element = array[i];
    if (%_Call(predicate, thisArg, element, i, array)) {
      return i;
    }
  }

  return -1;
}

// ES6 draft 07-15-13, section 15.4.3.24
function ArrayFindIndex(predicate, thisArg) {
  CHECK_OBJECT_COERCIBLE(this, "Array.prototype.findIndex");

  var array = TO_OBJECT(this);
  var length = TO_INTEGER(array.length);

  return InnerArrayFindIndex(predicate, thisArg, array, length);
}

// ES6, draft 04-05-14, section 22.1.3.6
function InnerArrayFill(value, start, end, array, length) {
  var i = IS_UNDEFINED(start) ? 0 : TO_INTEGER(start);
  var end = IS_UNDEFINED(end) ? length : TO_INTEGER(end);

  if (i < 0) {
    i += length;
    if (i < 0) i = 0;
  } else {
    if (i > length) i = length;
  }

  if (end < 0) {
    end += length;
    if (end < 0) end = 0;
  } else {
    if (end > length) end = length;
  }

  if ((end - i) > 0 && ObjectIsFrozen(array)) {
    throw MakeTypeError(kArrayFunctionsOnFrozen);
  }

  for (; i < end; i++)
    array[i] = value;
  return array;
}

// ES6, draft 04-05-14, section 22.1.3.6
function ArrayFill(value, start, end) {
  CHECK_OBJECT_COERCIBLE(this, "Array.prototype.fill");

  var array = TO_OBJECT(this);
  var length = TO_LENGTH_OR_UINT32(array.length);

  return InnerArrayFill(value, start, end, array, length);
}

function AddArrayElement(constructor, array, i, value) {
  if (constructor === GlobalArray) {
    %AddElement(array, i, value);
  } else {
    ObjectDefineProperty(array, i, {
      value: value, writable: true, configurable: true, enumerable: true
    });
  }
}

// ES6, draft 10-14-14, section 22.1.2.1
function ArrayFrom(arrayLike, mapfn, receiver) {
  var items = TO_OBJECT(arrayLike);
  var mapping = !IS_UNDEFINED(mapfn);

  if (mapping) {
    if (!IS_CALLABLE(mapfn)) {
      throw MakeTypeError(kCalledNonCallable, mapfn);
    }
  }

  var iterable = GetMethod(items, iteratorSymbol);
  var k;
  var result;
  var mappedValue;
  var nextValue;

  if (!IS_UNDEFINED(iterable)) {
    result = %IsConstructor(this) ? new this() : [];

    var iterator = GetIterator(items, iterable);

    k = 0;
    while (true) {
      var next = iterator.next();

      if (!IS_OBJECT(next)) {
        throw MakeTypeError(kIteratorResultNotAnObject, next);
      }

      if (next.done) {
        result.length = k;
        return result;
      }

      nextValue = next.value;
      if (mapping) {
        mappedValue = %_Call(mapfn, receiver, nextValue, k);
      } else {
        mappedValue = nextValue;
      }
      AddArrayElement(this, result, k, mappedValue);
      k++;
    }
  } else {
    var len = TO_LENGTH(items.length);
    result = %IsConstructor(this) ? new this(len) : new GlobalArray(len);

    for (k = 0; k < len; ++k) {
      nextValue = items[k];
      if (mapping) {
        mappedValue = %_Call(mapfn, receiver, nextValue, k);
      } else {
        mappedValue = nextValue;
      }
      AddArrayElement(this, result, k, mappedValue);
    }

    result.length = k;
    return result;
  }
}

// ES6, draft 05-22-14, section 22.1.2.3
function ArrayOf() {
  var length = %_ArgumentsLength();
  var constructor = this;
  // TODO: Implement IsConstructor (ES6 section 7.2.5)
  var array = %IsConstructor(constructor) ? new constructor(length) : [];
  for (var i = 0; i < length; i++) {
    AddArrayElement(constructor, array, i, %_Arguments(i));
  }
  array.length = length;
  return array;
}

// -------------------------------------------------------------------

%FunctionSetLength(ArrayCopyWithin, 2);
%FunctionSetLength(ArrayFrom, 1);
%FunctionSetLength(ArrayFill, 1);
%FunctionSetLength(ArrayFind, 1);
%FunctionSetLength(ArrayFindIndex, 1);

// Set up non-enumerable functions on the Array object.
utils.InstallFunctions(GlobalArray, DONT_ENUM, [
  "from", ArrayFrom,
  "of", ArrayOf
]);

// Set up the non-enumerable functions on the Array prototype object.
utils.InstallFunctions(GlobalArray.prototype, DONT_ENUM, [
  "copyWithin", ArrayCopyWithin,
  "find", ArrayFind,
  "findIndex", ArrayFindIndex,
  "fill", ArrayFill
]);

// -------------------------------------------------------------------
// Exports

utils.Export(function(to) {
  to.ArrayFrom = ArrayFrom;
  to.InnerArrayCopyWithin = InnerArrayCopyWithin;
  to.InnerArrayFill = InnerArrayFill;
  to.InnerArrayFind = InnerArrayFind;
  to.InnerArrayFindIndex = InnerArrayFindIndex;
});

})
