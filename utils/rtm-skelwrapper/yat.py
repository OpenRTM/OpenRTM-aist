#!/usr/bin/env python3
#
# @brief YAT: YAml Template text processor
# @date $Date: 2008-02-09 20:04:27 $
# @author Norkai Ando <n-ando@aist.go.jp>
#
# Copyright (C) 2008 Noriaki Ando, All rights reserved.
#
# $Id$
#

#
# Usage:
#------------------------------------------------------------
# import yaml
# import yat
#
# dict   = yaml.load(open(filename, "r").read())
# t      = yat.Template(template, "\[", "\]")
# result = t.generate(dict)
#------------------------------------------------------------
#
# 1. Simple directive:
#    [dictionary_key]
#
#    Nested dictionaries can be expressed by dotted expression.
#
# example:
# dict = {"a": "This is a",
#         "b": {"1": "This is b.1",
#               "2": "This is b.2"}
#        }
#
# template:
# [a]
#
# [b.1]
#
# [b.2]
#
# result:
# This is a
# This is b.1
# This is b.2
#
#
# 2. "for" directive:
#    [for key in list] statement [endfor]
#
#    Iterative evaluation for listed values is performed by "for" statement.
#    In iteration at each evaluation, the value of the list is assigned to
#    "key". The "key" also can be the nested dictionary directive.
#
# example:
# dict = {"list": [0, 1, 2],
#         "listed_dict": [
#           {"name": "x", "value": "1.0"},
#           {"name": "y", "value": "0.2"},
#           {"name": "z", "value": "0.1"}]}
#
# template:
# [for lst in list]
# [lst],  
# [endfor]
# [for lst in listed_dict]
# [lst.name]: [lst.value]
# 
# [endfor]
#
# result:
# 1, 2, 3,
# x: 1.0
# y: 0.2
# x: 0.1
#
#
# 3. "if-index" directive:
#    [for key in val]
#    [if-index key is first|even|odd|last|NUMBER] statement1
#    [elif-index key is first|even|odd|last|NUMBER] statement2
#    [endif][endfor]
#
#    "if-index" is used to specify the index of the "for" iteration.
#    The "key" string which is defined in the "for" statement is used as index.
#    A number or predefined directives such as "first", "even", "odd" and
#    "last" can be used to specify the index.
#
# example:
# dict = {"list": [0,1,2,3,4,5,6,7,8,9,10]}
#
# template:
# [for key in list]
# [if-index key is 3] [key] is hoge!!
# [elif-index key is 6] [key] is foo!!
# [elif-index key is 9] [key] is bar!!
# [elif-index key is first] [key] is first
# [elif-index key is last] Omoro-------!!!!
# [elif-index key is odd] [key] is odd number
# [elif-index key is even] [key] is even number
# [endif]
# [endfor]
#
# result:
#  0 is first
#  1 is odd number
#  2 is even number
#  3 is hoge!!
#  4 is even number
#  5 is odd number
#  6 is foo!!
#  7 is odd number
#  8 is even number
#  9 is bar!!
#  Omoro-------!!!!
#
#
# 4. "if" directive: [if key is value] text1 [else] text2 [endif]
#    If "key" is "value", "text1" appears, otherwise "text2" appears.
#
# example:
# dict = {"key1": "a", "key2": "b"}
#
# template:
# [if key1 is a]
# The key1 is "a".
# [else]
# This key1 is not "a".
# [endif]
#
# result:
# The key1 is "a".
#
#
# 5. "if-any" directive: [if-any key1] text1 [else] text2 [endif]
#    If the "key1" exists in the dictionary, "text1" appears, otherwise
#    "text2" appears.
#
# example:
# dict = {"key1": "a", "key2": "b"}
#
# template:
# [if-any key1]
# key1 exists.
# [endif][if-any key3]
# key3 exists.
# [else]
# key3 does not exists.
# [endif]
#
# result:
# key1 exists.
# key3 does not exists.
#
#
# 6. bracket and comment:
#    [[] is left bracket if begin mark is "["
#    [# comment ] is comment if begin/end marks are "[" and "]"
#
# example:
# dict = {}
#
# template:
# [[]bracket]
# [# comment]
#
# result:
# [bracket]
#
from __future__ import print_function
import string
import re
#from types import StringType, IntType, FloatType, DictType, ListType, ClassType
StringType = str
IntType = int
FloatType = float
DictType = dict
ListType = list
#ClassType = class
import sys

class Template:
    """
    usage:
      tempalte_text = read template text from file
      dictionary    = create dictionaly by using yaml
      t = Template(tempalte_text)
      generated_text = t.generate(dictionary)

    """
    
    def __init__(self, template, begin_mark="\\[", end_mark="\\]"):
        self.__procs = [self.__proc_text,
                        self.__proc_cmd,
                        self.__proc_bracket]
        self.template = template

        # regular expression to devide text into DIRECTIVE, BRACKET and COMMENT
        #
        # default:
        # START_MARK: "["
        # END_MARK  : "]"
        # -> START_MARK and END_MARK can be given in ctor
        #
        # ITEM: (?:"(?:[^\\"]|\\.)*"|[-\w.]+)
        # \[(ITEM(?: +ITEM)*)\]|(\[\[\])|\[#[^\]]*\]
        # ~~~~~~~~(1)~~~~~~ ~~(2)~~~ ~~~(3)~~~~~
        # (1) COMMAND  : '[' ITEM (whitespace ITEM)* ']
        #     ITEM     : STRING | NAME
        #     STRING   : '"' (not-slash-or-dquote | '\' anychar)* '"'
        #     NAME     : (alphanum | '_' | '-' | '.')+
        # (2) BEGIN_MARK_ESCAPE : '[[]'
        # (3) COMMENT  : '[#' not-rbracket
        #        
        # re_item      = r'(?:"(?:[^\\"]|\\.)*"|[-\w.]+)'
        # re_command   = r'\[(%s(?: +%s)*)\]' % (re_item, re_item)
        # re_beginmark = r'\[\[\]'
        # re_comment   = r'\[#[^\]]*\]'
        # re_parse     = re.compile(r'%s|(%s)|%s' 
        #                     % (re_command, re_beginmark, re_comment))
        # re_args      = re.compile(r'"(?:[^\\"]|\\.)*"|[-\w.]+')
        #
        #
        re_item      = r'(?:"(?:[^\\"]|\\.)*"|[-\w.:]+)'
        re_command   = r'%s(%s(?: +%s)*)%s' % \
            (begin_mark, re_item, re_item, end_mark)
        re_bracket   = r'%s%s%s' % \
            (begin_mark, begin_mark, end_mark)
        re_comment   = r'%s#[^%s]*%s' % \
            (begin_mark, end_mark, end_mark)
        self.begin_mark = begin_mark.replace("\\","")
        self.re_parse = re.compile(r'%s|(%s)|%s' % \
                                       (re_command, re_bracket, re_comment))
        self.re_args  = re.compile(r'"(?:[^\\"]|\\.)*"|[-\w.:]+')
        self.re_number = re.compile(r'[0-9]+')

        # tokenize input text
        self.token = self.re_parse.split(self.template)
        self.token_len  = len(self.token)
        
        # initialize variables
        self.script = program
        self.indent = 4
        self.script_level  = 2
        self.level = 0
        self.index = 0
        self.cmd_cxt = []

        # parse token
        self.__parse_template(self.token)

        return

    def generate(self, dict):
        # eval generated script
        exec(self.script, globals())
        # script includes Generator class
        gen = Generator(self.token, dict)
        # execute generated script
        return gen.generate()

    def get_script(self):
        return self.script

    def __push_level(self):
        self.level += 1

    def __pop_level(self):
        self.level -= 1

    def __write_cmd(self, cmd):
        tmp_cmd  = self.__indent()
        tmp_cmd += "self.set_index(%s)\n" % (self.index)
        self.script += tmp_cmd
        self.__write_cmd_noindex(cmd)

    def __write_cmd_noindex(self, cmd):
        tmp_cmd  = self.__indent()
        tmp_cmd += cmd + "\n"
        self.script += tmp_cmd

    def __parse_template(self, dict):
        try:
            # split into (TEXT DIRECTIVE BRACKET)* TEXT
            self.__parse()
        except YATException as e:
            self.__print_error(e)
            sys.exit(-1)

    def __indent(self):
        indent = " " * ((self.script_level + self.level) * self.indent)
        return indent

    def __parse(self):
        while self.index < self.token_len:
            self.__procs[self.index % 3]()
            self.index += 1

    def __proc_text(self):
        if self.token[self.index] == None:
            return
        cmd_text = "self.write_token(%s)" % (self.index)
        self.__write_cmd(cmd_text)
        return True
 
    def __proc_bracket(self):
        if self.token[self.index] == None:
            return
        cmd_text = "self.write(\"" + self.begin_mark + "\")"
        self.__write_cmd(cmd_text)
        return True
            
    def __proc_cmd(self):
        cmd = self.token[self.index]
        try:
            args = self.re_args.findall(cmd)
        except:
            return
        self.del_nl_after_cmd()
        argc = len(args)
        if argc == 0:
            raise InvalidDirective(self.lineno(), "_an empty directive_ ")

        # simple directive
        if argc == 1:
            if   args[0] == "endfor":
                self.__endfor_cmd(args)
                return
            elif args[0] == "else":
                self.__else_cmd(args)
                return
            elif args[0] == "last":
                self.__last_cmd(args)
                return
            elif args[0] == "endif":
                self.__endif_cmd(args)
                return
            else:
                self.__cmd(args)
                return
        elif argc == 2:
            if args[0] == "if-any":
                self.__if_any_cmd(args)
                return
        elif argc == 4: # [for key in value]
            if args[0] == "for" and args[2] == "in":
                self.__for_cmd(args)
                return True
            elif args[0] == "if" and args[2] == "is":
                self.__if_cmd(args)
            elif args[0] == "elif" and args[2] == "is":
                self.__elif_cmd(args)
            elif args[0] == "if-index" and args[2] == "is":
                self.__if_index_cmd(args)
            elif args[0] == "elif-index" and args[2] == "is":
                self.__elif_index_cmd(args)
            else:
                raise InvalidDirective(self.lineno(), cmd)
        else:
            raise InvalidDirective(self.lineno(), cmd)
        return True

    def __cmd(self, args):
        cmd_text = "self.write_dict(\"%s\")" % (args[0])
        self.__write_cmd(cmd_text)

    #------------------------------------------------------------
    # [for] commands
    # - for
    # - last
    # - endfor
    #------------------------------------------------------------
    def __for_cmd(self, args):
        """
        The following [for] directive
          [for tmp_key in directive]
        is converted into the following python command.
          for i in len(directive):
              self.dicts.append({tmp_key: ditective[i])
        and, endfor directive terminate as the following,
              self.dicts.pop()
        """
        key = args[1]
        directive = args[3]
        # (key)     : variable string of index variable for [for] block
        # (key)_list: list value of specified directive
        # (key)_len : length of the list
        cmd_text = "%s_list = self.get_list(\"%s\")" % (key, directive)
        self.__write_cmd(cmd_text)
        cmd_text = "%s_len = len(%s_list)" % (key, key)
        self.__write_cmd(cmd_text)
        cmd_text = "for %s_index in range(len(%s_list)):" % (key, key)
        self.__write_cmd(cmd_text)
        self.__push_level()
        cmd_text = "self.push_dict({\"%s\": %s_list[%s_index]})" \
            % (key, key, key)
        self.__write_cmd(cmd_text)
        self.cmd_cxt.append("for")

    def __endfor_cmd(self, args):
        try:
            cxt = self.cmd_cxt.pop()
            if cxt != "for":
                raise UnmatchedBlock(self.lineno(), "endfor")
            self.__write_cmd("self.pop_dict()")
            self.__pop_level()
        except:
            print(args, self.lineno())
            raise UnmatchedBlock(self.lineno(), "endfor")
        return

    # end of [for] commands
    #------------------------------------------------------------

    #------------------------------------------------------------
    # [if] commands
    # - if
    # - if-index
    # - if-any
    #------------------------------------------------------------
    def __if_cmd(self, args):
        """
        The following [if] directive
          [if directive is string]
        is converted into the following python command.
          if self.__get_string() == "string":
        """
        directive = args[1]
        string = args[3]
        cmd_text = "if self.get_text(\"%s\") == \"%s\":" % \
            (directive, string)
        self.__write_cmd(cmd_text)
        self.__push_level()
        self.cmd_cxt.append("if")
        return

    def __elif_cmd(self, args):
        if self.cmd_cxt[-1] != "if":
            raise UnmatchedBlock(self.lineno(), "elif")
        directive = args[1]
        string = args[3]
        cmd_text = "elif self.get_text(\"%s\") == \"%s\":" % \
            (directive, string)
        self.__pop_level()
        self.__write_cmd_noindex(cmd_text)
        self.__push_level()
        return

    # [if-index] commands
    def __if_index_cmd(self, args):
        # [if-index KEY is [first|even|odd|last|NUMBER]]
        #  ~~~0~~~  ~1~  2 ~~~~~~~~~~~~~~3~~~~~~~~~~~~
        cmdlist = {"first": "if %s_index == 0:",
                   "even" : "if (%s_index %% 2) == 0:",
                   "odd"  : "if (%s_index %% 2) != 0:",
                   "last" : "if %s_index == %s_len - 1:"}
        key = args[1]
        cmd = args[3]
        if len(self.re_number.findall(cmd)) == 1:
            cmd_text = "if %s_index == %s:" % (key, cmd)
        elif cmd in cmdlist:
            if cmd == "last":
                cmd_text = cmdlist[cmd] % (key,key)
            else:
                cmd_text = cmdlist[cmd] % (key)
        else:
            raise InvalidDirective(self.lineno(), ''.join(args))
        self.__write_cmd(cmd_text)
        self.__push_level()
        self.cmd_cxt.append("if-index")

    def __elif_index_cmd(self, args):
        if self.cmd_cxt[-1] != "if-index":
            raise UnmatchedBlock(self.lineno(), "elif-index")
        # [elif-index KEY is [first|even|odd|last|NUMBER]]
        #  ~~~0~~~  ~1~  2 ~~~~~~~~~~~~~~3~~~~~~~~~~~~
        cmdlist = {"first": "elif %s_index == 0:",
                   "even" : "elif (%s_index %% 2) == 0:",
                   "odd"  : "elif (%s_index %% 2) != 0:",
                   "last" : "elif %s_index == %s_len - 1:"}
        key = args[1]
        cmd = args[3]
        if len(self.re_number.findall(cmd)) == 1:
            cmd_text = "elif %s_index == %s:" % (key, cmd)
        elif cmd in cmdlist:
            if cmd == "last":
                cmd_text = cmdlist[cmd] % (key,key)
            else:
                cmd_text = cmdlist[cmd] % (key)
        else:
            raise InvalidDirective(self.lineno(), ' '.join(args))
        self.__pop_level()
        self.__write_cmd_noindex(cmd_text)
        self.__push_level()

    # [if-any] command
    def __if_any_cmd(self, args):
        directive = args[1]
        cmd_text = "if self.has_key(\"%s\"):" % (directive)
        self.__write_cmd(cmd_text)
        self.__push_level()
        self.cmd_cxt.append("if-any")
        return

    def __elif_any_cmd(self, args):
        if self.cmd_cxt[-1] != "if-any":
            raise UnmatchedBlock(self.lineno(), "elif-any")
        directive = args[1]
        cmd_text = "if self.has_key(\"%s\"):" % (directive)
        self.__pop_level()
        self.__write_cmd_noindex(cmd_text)
        self.__push_level()
        return

    # [else], [endif] commands
    def __else_cmd(self, args):
        if self.cmd_cxt[-1] != "if" and self.cmd_cxt[-1] != "if-index" \
                and self.cmd_cxt[-1] != "if-any":
            raise UnmatchedBlock(self.lineno(), "else")
        self.__pop_level()
        self.__write_cmd_noindex("else:")
        self.__push_level()
        return

    def __endif_cmd(self, args):
        if self.cmd_cxt[-1] != "if" and self.cmd_cxt[-1] != "if-index" \
                and self.cmd_cxt[-1] != "if-any":
            raise UnmatchedBlock(self.lineno(), "endif")
        self.cmd_cxt.pop()
        self.__pop_level()
        return
    # end of [if] commands
    #------------------------------------------------------------

    def __print_error(self, e):
        print("Parse Error: line", e.lineno, "in input data", file=sys.stderr)
        print("  " + ''.join(nesteditem(e.value)), file=sys.stderr)
        lines = self.template.split("\n")
        length = len(lines)
        print("------------------------------------------------------------", file=sys.stderr)
        for i in range(1,10):
            l = e.lineno - 6 + i
            if l > 0 and l < length:
                print(lines[l], file=sys.stderr)
                if i == 5:
                    uline = '~'*len(lines[l])
                    print(uline, file=sys.stderr)
        print("------------------------------------------------------------", file=sys.stderr)
    
    def del_nl_after_cmd(self):
        # next text index after command
        next = self.index + 2
        if next > self.token_len: return
        if self.token[next] == None: return
        text = self.token[next]
        tlen = len(text)
        if tlen > 0 and text[0] == '\n':
            self.token[next] = text[1:]
            return
        elif tlen > 0 and text[0] == '\r':
            self.token[next] = text[1:]
            return
        elif tlen > 1 and text[0:2] == '\r\n':
            self.token[next] = text[2:]

    def lineno(self):
        l = 1
        for i in range(self.index):
            if isinstance(self.token[i], StringType):
                l += self.token[i].count('\n')
        for i in range(1, self.index, 3):
            l += 1
        return l


#------------------------------------------------------------
# Generator and GeneratorBase classes
#------------------------------------------------------------
program = """
class Generator(GeneratorBase):
    def __init__(self, token, dict):
        GeneratorBase.__init__(self, token, dict)
    def generate(self):
        try:
            self.process()
        except YATException as e:
            self.print_error(e)
            sys.exit(-1)
        return self.text

    def process(self):
"""

class GeneratorBase:
    def __init__(self, token, dict):
        self.token = token
        self.dicts = [dict]
        self.index = 0
        self.text = ""

    def print_error(self, e):
        print("\nTemplate Generation Error: line", e.lineno, "in input data", file=sys.stderr)
        print("  " + ''.join(nesteditem(e.value)), file=sys.stderr)
        temp = ""
        for i, s in enumerate(self.token):
            if s != None:
                if i % 3 == 1:
                    temp += "[" + s + "]\n"
                else:
                    temp += s
        lines = temp.split("\n")
        length = len(lines)
        print("------------------------------------------------------------", file=sys.stderr)
        for i in range(1,10):
            l = e.lineno - 6 + i
            if l > 0 and l < length:
                print(lines[l], file=sys.stderr)
                if i == 5:
                    uline = '~'*len(lines[l])
                    print(uline, file=sys.stderr)
        print("------------------------------------------------------------", file=sys.stderr)
        
    def set_index(self, index):
        self.index = index

    def push_dict(self, dict):
        self.dicts.append(dict)

    def pop_dict(self):
        if len(self.dicts) < 2:
            raise UnmatchedBlock(self.lineno(), "")
        self.dicts.pop()

    def write(self, text):
        self.text += text

    def write_dict(self, keytext):
        self.write(self.get_text(keytext))

    def write_token(self, index):
        self.write(self.token[index])

    def lineno(self):
        cnt = 1
        for i in range(0, self.index, 3):
            if self.token[i] != None:
                cnt += self.token[i].count('\n')
        # count deleted '\n' after commands
        for i in range(1, self.index, 3):
            if self.token[i] != None:
                cnt += 1
        return cnt
                                
    def get_text(self, keytext):
        val = self.get_value(keytext)
        if isinstance(val, StringType):
            return val
        if isinstance(val, IntType) or isinstance(val, FloatType):
            return str(val)
        raise UnexpectedData(self.lineno(), "\"" + keytext + \
                                 "\" should have string, int or float value.")

    def get_list(self, keytext):
        val = self.get_value(keytext)
        if not isinstance(val, ListType):
            raise UnexpectedData(self.lineno(),
                                 "\"" + keytext + "\" should have list value.")
        return val

    def has_key(self, keytext):
        try:
            self.get_value(keytext)
            return True
        except NotFound as e:
            return False

    def get_value(self, keytext):
        keys = keytext.split('.')
        for i in range(len(self.dicts) - 1, -1, -1):
            dict_value = self.get_dict_value(keys, self.dicts[i])
            if dict_value != None:
                return dict_value
        raise NotFound(self.lineno(), keytext) 

    def get_dict_value(self, keys, dict):
        length = len(keys)
        d = dict
        for i in range(length):
            if isinstance(d, DictType) and keys[i] in d:
                d = d[keys[i]]
            else:
                return None
        return d


#------------------------------------------------------------
# Exceptions                                
#------------------------------------------------------------
class YATException(Exception):
    pass

class UnknownError(YATException):
    def __init__(self, lineno):
        self.lineno = lineno
        self.value = "Unknown error."

class UnmatchedBlock(YATException):
    def __init__(self, lineno, msg):
        self.lineno = lineno
        self.value = "Unmatched block error: " + msg

class UnexpectedData(YATException):
    def __init__(self, lineno, msg):
        self.lineno = lineno
        self.value = msg

class NotFinalElement(YATException):
    def __init__(self, dictkey, dictvalue):
        self.value = "Specified key is not final element: ",\
            dictkey, "=>", dictvalue

class InvalidDirective(YATException):
    def __init__(self, lineno, directive):
        self.lineno = lineno
        self.value = "Invalid directive: \"[" + directive + "]\""

class UnmatchedData(YATException):
    def __init__(self, lineno, description):
        self.lineno = lineno
        self.value = "Unmatched data and input: ", description

class NotFound(YATException):
    def __init__(self, lineno, description):
        self.lineno = lineno
        self.value = "Value not found for: \"" + description + "\""

#------------------------------------------------------------
# other functions
#------------------------------------------------------------
def nesteditem(aList):
    for anItem in aList:
        if type(anItem)==list:
            for subitem in nesteditem(anItem):
                yield subitem
        else:
            yield anItem



if __name__ == "__main__":
    dict = []
    template = []
    #------------------------------------------------------------
    # Example 0
    #------------------------------------------------------------
    dict.append({"a": "This is a",
                 "b": {"1": "This is b.1",
                       "2": "This is b.2"}
                 })
    template.append("""[a]

[b.1]

[b.2]""")

    #------------------------------------------------------------
    # Example 1
    #------------------------------------------------------------
    dict.append({"list": [0, 1, 2],
                 "listed_dict": [
                {"name": "x", "value": "1.0"},
                {"name": "y", "value": "0.2"},
                {"name": "z", "value": "0.1"}]})
    template.append("""[for lst in list]
[lst],  
[endfor]
[for lst in listed_dict]
[lst.name]: [lst.value]

[endfor]""")

    #------------------------------------------------------------
    # Example 2
    #------------------------------------------------------------
    dict.append({"list": [0,1,2,3,4,5,6,7,8,9,10]})
    template.append("""[for key in list]
[if-index key is 3] [key] is hoge!!
[elif-index key is 6] [key] is foo!!
[elif-index key is 9] [key] is bar!!
[elif-index key is first] [key] is first
[elif-index key is last] Omoro-------!!!!
[elif-index key is odd] [key] is odd number
[elif-index key is even] [key] is even number
[endif]
[endfor]""")

    #------------------------------------------------------------
    # Example 3
    #------------------------------------------------------------
    dict.append({"key1": "a", "key2": "b"})
    template.append("""[if key1 is a]
The key1 is "a".
[else]
This key1 is not "a".
[endif]""")

    #------------------------------------------------------------
    # Example 4
    #------------------------------------------------------------
    dict.append({"key1": "a", "key2": "b"})
    template.append("""[if-any key1]
key1 exists.
[endif][if-any key3]
key3 exists.
[else]
key3 does not exists.
[endif]""")

    dict.append({})
    template.append("""
[[]bracket]
[# comment]
""")

    import yaml
    if len(dict) == len(template):
        for i in range(len(dict)-1,len(dict)):
            t = Template(template[i])
            print("-" * 60)
            print("Example:", i)
            print("-" * 60)
            print("Template:\n")
            print(template[i])
            print("-" * 60)
            print("Dictionary:\n")
            print(yaml.dump(dict[i], default_flow_style=False))
            print("-" * 60)
            print("Generated Script:\n")
            print(t.get_script())
            print("-" * 60)
            print("Generated Text:\n")
            print(t.generate(dict[i]))
            print("")
