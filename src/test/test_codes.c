/*
 * Copyright (c) 2011 Ignasi Barrera
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "minunit.h"
#include "test.h"
#include "../lib/codes.h"


char* test_is_error() {
    // Valid error codes
    mu_assert(is_error("400") == 1, "test_is_error: 400 should be a valid error");
    mu_assert(is_error("427") == 1, "test_is_error: 427 should be a valid error");
    mu_assert(is_error("599") == 1, "test_is_error: 599 should be a valid error");

    // Invalid error codes
    mu_assert(is_error("200") == 0, "test_is_error: 200 should not be a valid error");
    mu_assert(is_error("399") == 0, "test_is_error: 299 should not be a valid error");
    mu_assert(is_error("600") == 0, "test_is_error: 600 should not be a valid error");
    
    return 0;
}

char* test_is_numeric_response() {
    // Valid reponse codes
    mu_assert(is_numeric_response("200") == 1, "test_is_numeric_response: 200 should be a valid response");
    mu_assert(is_numeric_response("399") == 1, "test_is_numeric_response: 399 should be a valid response");
    mu_assert(is_numeric_response("250") == 1, "test_is_numeric_response: 250 should be a valid response");

    // Invalid error codes
    mu_assert(is_numeric_response("199") == 0, "test_is_numeric_response: 199 should not be a valid response");
    mu_assert(is_numeric_response("400") == 0, "test_is_numeric_response: 400 should not be a valid response");
    mu_assert(is_numeric_response("403") == 0, "test_is_numeric_response: 403 should not be a valid response");

    return 0;
}

void test_codes() {
    mu_run(test_is_error);
    mu_run(test_is_numeric_response);
}

