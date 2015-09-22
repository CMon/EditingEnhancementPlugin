# EditingEnhancementPlugin
A Plugin for QtCreator which enhances the editing by some commands that are not possible otherwise

## Compilation
Setup the environment variables:
* QTC_SOURCE has to point to the QtCreator source
* QTC_BUILD has to point to the QtCreator build

## Installation
Look into the editingenhancement.pro file to get instructions about where to put the builded plugin.

## New Commands
* Sort paragraph: CTRL+SHIFT+E,CTRL+SHIFT+S
** This sorts all lines on the current paragraph. A paragraphs delimiter is an empty or only with whitespaces filled line. (There is a similar method within QtCreator already but that one only works if you select lines, this one does
without a selection
* Sort indented paragraph: CTRL+E,CTRL+S
** This sorts all lines on the current paragraph, similar to the previous call but this time the paragraph ends when the indentation stops. This is helpfull if you want to order stuff like the "HEADER += \" entries in the .pro file.
* Advanced Alignment: CTRL+E,CTRL+R
** This vertically aligns by a common found object. You select from the First char you want the lines align to to the last char it should align to and then press the combo.

## new QuickFix
* When hitting ALT+Enter and if you are within an if statement you are now able to add or remove a '!' infront of the current statement

