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
#define USE_INIT2
#include "demos.h"
#include <ctype.h>
#include <string.h>
#include <time.h>

const char *hangman_words[] = {
	"algorithm",   "bytecode", "compiler", "debugging", "encryption",
	"firewall",	   "graphics", "hacker",   "input",		"javascript",
	"kernel",	   "loop",	   "malware",  "network",	"operating",
	"protocol",	   "queue",	   "robotics", "software",	"terminal",
	"unix",		   "virtual",  "web",	   "xml",		"yield",
	"zip",		   "assembly", "binary",   "cache",		"database",
	"ethernet",	   "firewire", "gigabyte", "hashing",	"interrupt",
	"java",		   "kernel",   "logic",	   "mainframe", "namespace",
	"output",	   "program",  "query",	   "router",	"storage",
	"transaction", "unicode",  "viewport", "wireless",	"xor",
};

const char *hangman_hints[] = {
	"A step-by-step problem-solving process.",
	"Low-level representation of code executed by a virtual machine.",
	"Translates high-level code into machine code.",
	"Finding and fixing errors in code.",
	"Converting data into a secret code to secure it.",
	"A barrier that prevents unauthorized access.",
	"Creating and manipulating visual images.",
	"An individual who breaks into computer systems.",
	"User input to a computer system.",
	"A scripting language commonly used for web development.",
	"The core component of an operating system.",
	"A sequence of instructions that repeats.",
	"Software designed to harm or exploit computer systems.",
	"A network of interconnected devices.",
	"Managing computer hardware and software resources.",
	"A set of rules for data communication.",
	"A data structure that follows the FIFO principle.",
	"The design and creation of robots.",
	"Instructions and data that tell a computer what to do.",
	"A text-based interface to interact with a computer.",
	"An operating system known for its multitasking capabilities.",
	"Simulated environment that is not physically present.",
	"A markup language for structuring and presenting content.",
	"To produce or provide a yield.",
	"A file compression format.",
	"Base-2 numeral system.",
	"Low-level programming language.",
	"High-speed data storage.",
	"Structured collection of data.",
	"A means of connecting devices to a network.",
	"High-speed data transfer interface.",
	"A unit of digital information.",
	"Converting data into a fixed-size string of characters.",
	"An event that interrupts normal processing.",
	"Object-oriented programming language.",
	"The core part of an operating system.",
	"A method of reasoning used in programming.",
	"A large, powerful computer used for complex tasks.",
	"A container for organizing code elements.",
	"Data generated by a program.",
	"A set of instructions executed by a computer.",
	"A request for information from a database.",
	"Directs data between networks.",
	"A place to store data or belongings.",
	"An exchange of information or currency.",
	"A character encoding standard.",
	"A visible area of a user interface.",
	"Communication without physical connections.",
	"A bitwise operation.",
};

EM_JS(int, random_of, (int of), {return Math.random() * (of - 1) | 0})

struct {
	const char *path;
	ImTextureID tid;
	int w, h;
} hangman_states[] = {
	{.path = "hm-0.png"},
	{.path = "hm-1.png"},
	{.path = "hm-2.png"},
	{.path = "hm-3.png"},
	{.path = "hm-4.png"},
	{.path = "hm-5.png"},
	{.path = "hm-6.png"},
	{.path = "hm-7.png"},
};

void init2(void) {
	for (int i = 0; i < IM_ARRAYSIZE(hangman_states); i++) {
		int w, h;
		
		char *data = emscripten_get_preloaded_image_data(hangman_states[i].path, &w, &h);

		printf("image(\"%s\", %d, %d): %p\n", hangman_states[i].path, w, h, data);

		sg_image sg_img = sg_make_image(&(sg_image_desc){
			.width = w,
			.height = h,
			.pixel_format = SG_PIXELFORMAT_RGBA8,
			.wrap_u = SG_WRAP_CLAMP_TO_EDGE,
			.wrap_v = SG_WRAP_CLAMP_TO_EDGE,
			.min_filter = SG_FILTER_LINEAR, 
			.mag_filter = SG_FILTER_LINEAR, 
			.data.subimage[0][0] = {
				.ptr = data,
				.size = (size_t)w * h * 4,
			}
		});

		hangman_states[i].tid = (ImTextureID)(uintptr_t)sg_img.id;
		hangman_states[i].w = w;
		hangman_states[i].h = h;
	}
}

float calculate_tax(float income) {
    if (income <= 18200) {
        return 0;
    } else if (income <= 45000) {
        return (income - 18200) * 0.19;
    } else if (income <= 120000) {
        return 5092 + (income - 45000) * 0.325;
    } else if (income <= 180000) {
        return 29467 + (income - 120000) * 0.37;
    } else {
        return 51667 + (income - 180000) * 0.45;
    }
}

void tax_breaks(void) {
    // Set initial window position and size
    igSetNextWindowPos((ImVec2){10, 10}, ImGuiCond_Once, (ImVec2){0, 0});
    igSetNextWindowSize((ImVec2){380.f, 400.f}, ImGuiCond_Once);
    
    // Begin ImGui window titled "Tax Breaks"
    igBegin("Tax Breaks", 0, ImGuiWindowFlags_AlwaysAutoResize);
    
    // Buffer for employee name input, income input, and output
    static char name_input_buf[128] = {};
    static char income_input_buf[128] = {};
    static char output_buf[1024] = {};

    // Flag to determine whether to add a new entry
    bool add_entry = false;

    // Begin an ImGui table with 3 columns for entry input
    if (igBeginTable("Entry", 3, 0, V2ZERO, 20.0f)) {
        // Move to the next row in the table
        igTableNextRow(0, 0);
        
        // First column: Employee Name input
        igTableNextColumn();
        {
            igInputTextEx("Employee Name", "", name_input_buf, sizeof(name_input_buf), (ImVec2){100.f, 0.f}, 0, 0, 0);
        }
        
        // Second column: Gross Income input
        igTableNextColumn();
        {
            igInputTextEx("Gross Income", "", income_input_buf, sizeof(income_input_buf), (ImVec2){100.f, 0.f}, ImGuiInputTextFlags_CharsDecimal, 0, 0);
        }
        
        // Third column: "Add Entry" button
        igTableNextColumn();
        {
            add_entry = igButton("Add Entry", V2ZERO);
        }
        
        // End the table
        igEndTable();
    }

    // Initialize arrays to hold entry information
    static int entries_count = 0;
    static const char *name_entries[20];
    static double income_entries[20];
    static double tax_entries[20];

    // If "Add Entry" button is pressed and there is space for new entries
    if (add_entry && entries_count < 20) {		
        // Convert income input to double
        char *tail_err;
        double income = strtod(income_input_buf, &tail_err);

        // If name or income is provided, add an entry
        if (name_input_buf[0] != '\0' || income_input_buf[0] != '\0') {
            name_entries[entries_count] = strdup(name_input_buf);
            income_entries[entries_count] = income;
            tax_entries[entries_count] = calculate_tax(income);

            entries_count++;
        }

        // Clear input buffers
        memset(name_input_buf, 0, sizeof(name_input_buf));
        memset(income_input_buf, 0, sizeof(income_input_buf));
    }

    // Create a separator for visual clarity
    igSeparator();

    // Button to display tax information
    if (igButton("Display Tax", V2ZERO)) {
        int output_p = 0;

        // Generate output for each entry and calculate total tax
        for (int i = 0; i < entries_count; i++) {
            output_p += snprintf(output_buf + output_p, sizeof(output_buf) - output_p, "%s - Income: %g, Taxation: $%g\n", name_entries[i], income_entries[i], tax_entries[i]);
        }

        // Free allocated memory for name entries and reset entries count
        for (int i = 0; i < entries_count; i++) {
            free((void*)name_entries[i]);
        }
        entries_count = 0;
    }

    // Display the count of entries
    igSameLine(0, igGetStyle()->CellPadding.x);
    igText("Entries: %d", entries_count);

    // Create another separator
    igSeparator();

    // Get available region for output display
    ImVec2 avail;
    igGetContentRegionAvail(&avail);

    // Multiline text input for displaying output
    igInputTextMultiline("##output_box", output_buf, sizeof(output_buf), (ImVec2){avail.x, 200.f}, 0, 0, 0);

    // End the ImGui window
    igEnd();
}

void hangman(void) {
    // Define enumeration to represent different hangman colour states
    enum hangman_colour_state {
        ST_FINE,
        ST_FAILED,
        ST_SUCCESS,
    };

    // Static variable to store the current hangman colour state
    static enum hangman_colour_state state = ST_FINE;

    // Set window background colour based on the current state
    switch (state) {
        case ST_FINE: break;
        case ST_FAILED:
            igPushStyleColor_U32(ImGuiCol_WindowBg, IM_COL32(255, 0, 0, 255));
            break;
        case ST_SUCCESS:
            igPushStyleColor_U32(ImGuiCol_WindowBg, IM_COL32(0, 255, 0, 255));
            break;
    }

    // Set initial window position and size
    igSetNextWindowPos((ImVec2){10, 10}, ImGuiCond_Once, (ImVec2){0, 0});
    igSetNextWindowSize((ImVec2){380.f, 400.f}, ImGuiCond_Once);
    
    // Begin ImGui window titled "Hangman"
    igBegin("Hangman", 0, ImGuiWindowFlags_AlwaysAutoResize);

    // Reset window background if not in ST_FINE state
    if (state != ST_FINE) {
        igPopStyleColor(1);
        state = ST_FINE;
    }

    // Temporary buffer for formatting text
    char buf[128];

    // Get available content region
    ImVec2 avail;
    igGetContentRegionAvail(&avail);

    // Explanatory text for the interface
    const char* explanation1 = "+ Textboxes for Incomplete Word and Missed Letters:\n\n"
                              "These textboxes provide real-time game information. The 'Incomplete Word' aids \n"
                              "the player's progress, while 'Missed Letters' prevents repeated guesses, \n"
                              "enhancing engagement. In this Hangman program, they foster strategy and \n"
                              "informed guessing, making the game more interactive.";
    const char* explanation2 = "+ Restart Button:\n\n"
                              "The 'Restart?' button ensures seamless user experience by enabling game \n"
                              "restarts without program relaunch. Enhancing usability, it aligns with user \n"
                              "expectations and optimises efficiency. Its inclusion reflects general interface \n"
                              "design principles and enhances user-friendliness in the program.";

    // Flag to track if interface info is selected
    static bool interface_info_selected = false;
    igSelectable_BoolPtr("Click For Interface Information", &interface_info_selected, 0, V2ZERO);

    // Display interface information as a popup if selected
    if ((interface_info_selected = igBeginPopupContextItem("", 0))) {
        igText(explanation1);
        igSeparator();
        igText(explanation2);
        igEndPopup();
    }

    // Create a separator for visual clarity
    igSeparator();

    // Flags to track game state
    bool refresh = false;
    bool failed = false;
    bool found = false;

    // Static variables to manage game data and state
    static int word_idx = -1;
    static const char *selected_word;
    static unsigned selected_word_len = 0;
    static char selbuf[64];
    static unsigned found_chars;
    static char missed_letters[64];
    static unsigned missed_letters_count = 0;

    // Initialize game data if word index is -1
    if (word_idx == -1) {
        word_idx = random_of(IM_ARRAYSIZE(hangman_words));
        selected_word = hangman_words[word_idx];
        selected_word_len = strlen(selected_word);
        memset(selbuf, '_', selected_word_len);
        selbuf[selected_word_len] = 0;
        found_chars = 0;
        memset(missed_letters, 0, IM_ARRAYSIZE(missed_letters));
        missed_letters_count = 0;
    }

    // Determine hangman state based on missed letters count
    int hangman_state = missed_letters_count;
    if (missed_letters_count >= IM_ARRAYSIZE(hangman_states)) {
        hangman_state = IM_ARRAYSIZE(hangman_states) - 1;
    }

    // Get ImGui IO for input handling
    ImGuiIO *io = igGetIO();

    // Update failed and found states
    failed = found_chars != selected_word_len && missed_letters_count >= IM_ARRAYSIZE(hangman_states) - 1;
    found = found_chars == selected_word_len;

    // Handle user input for the game
    if (!failed && !found) {
        unsigned short ch = 0;
        if (igIsWindowFocused(0) && io->InputQueueCharacters.Size > 0) {
            ch = io->InputQueueCharacters.Data[0];
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
                ch = tolower(ch);
            } else {
                ch = 0;
            }
        }
        if (ch != 0 && !strchr(selbuf, ch)) {
            bool ever_found = false;

            for (unsigned i = 0; i < selected_word_len; i++) {
                if (selected_word[i] == ch) {
                    selbuf[i] = ch;
                    found_chars++;
                    ever_found = true;
                    state = ST_SUCCESS;
                }
            }
            if (!ever_found && !strchr(missed_letters, ch)) {
                state = ST_FAILED;
                missed_letters[missed_letters_count++] = ch;
            }
        }
    }

    // Determine image size based on hangman state
    ImVec2 img_size = {(float)hangman_states[hangman_state].w * 1.5f, (float)hangman_states[hangman_state].h * 1.5f};
    
    // Center-align image within the available region
    igSetCursorPosX(igGetCursorPosX() + (avail.x - img_size.x) * 0.5f);
    // Display hangman image
    igImage(hangman_states[hangman_state].tid, img_size, (ImVec2){0.f, 0.f}, (ImVec2){1.f, 1.f}, (ImVec4){1.0f, 1.0f, 1.0f, 1.0f}, (ImVec4){});

    // Create a separator for visual clarity
    igSeparator();
    {
        ImVec2 box_size;

        // Format and display the incomplete word
        snprintf(buf, sizeof(buf), "word: %s", selbuf);
        box_size = nice_box_size(buf);
        igSetCursorPosX(igGetCursorPosX() + (avail.x - box_size.x) * 0.5f);
        nice_box(buf, IM_COL32(36, 36, 36, 255));

        // Format and display the hint for the selected word
        snprintf(buf, sizeof(buf), "hint: %s", hangman_hints[word_idx]);
        box_size = nice_box_size(buf);
        igSetCursorPosX(igGetCursorPosX() + (avail.x - box_size.x) * 0.5f);
        nice_box(buf, IM_COL32(36, 36, 36, 255));
    }
    // Create a separator for visual clarity
    igSeparator();

    // Button text for restart or game status
    const char *button_text = "Restart?";

    // Display restart button or game status based on conditions
    if (!(failed || found)) {
        if (failed) {
            strncpy(buf, "failed!", sizeof(buf));
        } else if (found) {
            strncpy(buf, "found!", sizeof(buf));
        } else {
            snprintf(buf, sizeof(buf), "missed: %s", missed_letters);
        }

        // Display game status message in a nice box
        float cpy = igGetCursorPosY();
        nice_box(buf, IM_COL32(36, 36, 36, 255));

        // Calculate button width and display the restart button
        ImVec2 text_size;
        igCalcTextSize(&text_size, button_text, NULL, true, -1.0f);
        float button_width = text_size.x + igGetStyle()->FramePadding.x * 2.f;
        igSetCursorPosY(cpy);
        igSetCursorPosX(igGetCursorPosX() + avail.x - button_width);

        // Trigger game restart on button press
        if (igButton(button_text, V2ZERO)) {
            refresh = true;
        }
    } else {
        // Display the restart button when game is over
        if (igButton(button_text, (ImVec2){avail.x, 0.f})) {
            refresh = true;
        }
    }

    // Refresh the game state if restart button is pressed
    if (refresh) {
        word_idx = -1;
    }

    // End the ImGui window
    igEnd();
}

void frame(void) {
	FRAME_PASS_BEGIN;

	hangman();
	tax_breaks();

	BLIT_BG(IM_COL32(50, 50, 50, 255));

	igShowDemoWindow(false);

	FRAME_PASS_END;
}