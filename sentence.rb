require 'Set'

def self.find_sentence?(str, dict, sentence = [], bad_prefixes = Set.new)
  is_sentence_found = false
  return true if str.empty?
  return false if (true == bad_prefixes.include?(str))
  dict.map{ |x| x }.each{ |word|
    next if !str.start_with?(word)
    cur_word_cnt = sentence.size
    sentence << word
    is_sentence_found = find_sentence?(
      str[word.size..-1], dict.delete(word), sentence, bad_prefixes
    )
    break if (true == is_sentence_found)
    dict.add(word)
    bad_prefixes.add(sentence.join)
    (sentence.size - cur_word_cnt).times{ |i| sentence.pop }
  }
  return is_sentence_found
end

ret = []
find_sentence?('acatsanddog', Set.new(['c', 'cat', 'cats', 'and', 'dog', 'a']), ret)
puts ret.inspect
