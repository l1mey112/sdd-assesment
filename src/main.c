/*
 * Copyright (C) 2022 l-m.dev
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "demos.h"
#include <ctype.h>
#include <time.h>
#include <string.h>

const char *hangman_words[] = {
	"algorithm",
	"programming",
	"database",
	"encryption",
	"compiler",
	"interface",
	"allocation",
	"recursion",
	"networking",
	"artificial"
};

const char *hangman_hints[] = {
	"A step-by-step procedure or formula used to solve a problem.",
	"The process of writing, testing, and maintaining instructions.",
	"A structured collection of data that is organized and accessible.",
	"The process of converting plaintext into ciphertext for security.",
	"A software tool that translates high-level code into machine code.",
	"A point of interaction between different software components.",
	"The process of assigning and reserving memory or resources.",
	"A programming technique where a function calls itself.",
	"The practice of connecting multiple computers and devices.",
	"A branch of computer science dealing with machine intelligence."
};

EM_JS(int, random_of, (int of), {
	return Math.random() * (of - 1) | 0
})

int select_random_word_idx(void) {
	int idx = random_of(IM_ARRAYSIZE(hangman_words));

	return idx;
}

int filter_to_az(ImGuiInputTextCallbackData* data) {
    char ch = (char)data->EventChar;
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
        return 0;
    return 1;
}

void hangman(void) {
	bool refresh = false;

	static int word_idx = -1;
	static const char *selected_word;
	static unsigned selected_word_len = 0;
	static char selbuf[64];
	static unsigned found_chars;
	static char missed_letters[64];
	static unsigned missed_letters_count = 0;

	if (word_idx == -1) {
		word_idx = select_random_word_idx();
		selected_word = hangman_words[word_idx];
		selected_word_len = strlen(selected_word);
		memset(selbuf, '_', selected_word_len);
		selbuf[selected_word_len] = 0;
		found_chars = 0;
		memset(missed_letters, 0, IM_ARRAYSIZE(missed_letters));
		missed_letters_count = 0;
	}

	ImGuiIO* io = igGetIO();

	if (found_chars != selected_word_len) {
		unsigned short ch = 0;
		if (io->InputQueueCharacters.Size > 0) {
			ch = io->InputQueueCharacters.Data[0];
			if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
				ch = tolower(ch);
			} else {
				ch = 0;
			}
		}
		// if `ch` isn't nul && `ch` hasn't been found yet
		if (ch != 0 && !strchr(selbuf, ch)) {
			bool ever_found = false;

			for (unsigned i = 0; i < selected_word_len; i++) {
				if (selected_word[i] == ch) {
					// found
					selbuf[i] = ch;
					found_chars++;
					ever_found = true;
				}
			}
			// append to missed letters if never found and never missed before
			if (!ever_found && !strchr(missed_letters, ch)) {
				missed_letters[missed_letters_count++] = ch;
				igText("go!");
			}
		}
		igText("missed: %s", missed_letters);
	} else {
		igText("found!");
	}
	if (igButton("refresh everything", V2ZERO)) {
		refresh = true;
	}

	igTextWrapped("hint: %s", hangman_hints[word_idx]);
	igTextWrapped("%s", selbuf);
	
	if (refresh) {
		word_idx = -1;
	}
}

void frame(void)
{
	FRAME_PASS_BEGIN;

	igSetNextWindowPos((ImVec2){10, 10}, ImGuiCond_Once, (ImVec2){0, 0});
	igSetNextWindowSize((ImVec2){400.f, 400.f}, ImGuiCond_Once);
	igBegin("Hangman", 0, ImGuiWindowFlags_NoResize);
	hangman();
	igEnd();

	BLIT_BG(IM_COL32(50, 50, 50, 255));

	igShowDemoWindow(false);

	FRAME_PASS_END;
}