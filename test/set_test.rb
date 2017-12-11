require 'test_helper'

class IntegerSet::SetTest < Minitest::Test
  def setup
    @set = IntegerSet::Set.new(10)
    @max_size = 10
  end

  def test_max_size
    assert_equal @max_size, @set.max_size
  end

  def test_cannot_add_non_numbers
    assert_raises(TypeError) { @set.add('hi') }
    assert_raises(TypeError) { @set.add(nil) }
    assert_raises(TypeError) { @set.add(Object.new) }
  end

  def test_initializes_empty
    refute((0..@max_size).any?(&@set.method(:include?)))
  end

  def test_add_and_test_for_existence
    @set.add(0)
    @set.add(4)
    @set.add(7)
    assert @set.include?(0)
    assert @set.include?(4)
    assert @set.include?(7)
    refute [1, 2, 3, 5, 6, 8, 9].any?(&@set.method(:include?))
  end

  def test_add_and_include_aliases
    @set << 0
    @set << 4
    @set << 7
    assert @set.member?(0)
    assert @set.member?(4)
    assert @set.member?(7)
  end

  def test_each
    @set = IntegerSet::Set.new(10)
    @set.add(0)
    @set.add(4)
    @set.add(7)
    results = []
    @set.each do |n|
      results << n
    end
    assert_equal [0, 4, 7], results
  end

  def test_implements_enumerable
    @set.add(0)
    @set.add(4)
    @set.add(7)
    assert_equal([1, 5, 8], @set.map { |x| x + 1 })
  end
end
