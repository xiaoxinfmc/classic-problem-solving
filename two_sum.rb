# @param {Integer[]} nums
# @param {Integer} target
# @return {Integer[]}
def two_sum(nums, target)
    existance_map = {}; pair_to_ret = []
    nums.each_with_index{ |val, idx| existance_map[val] = idx }
    nums.each_with_index{ |val, idx|
        id_to_pair = existance_map[target - val]
        next if (id_to_pair.nil? || id_to_pair == idx)
        pair_to_ret = [idx, id_to_pair]
        break
    }
    return pair_to_ret
end

puts two_sum([3, 2, 4], 6)
