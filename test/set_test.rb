require 'test_helper'

class IntegerSet::SetTest < Minitest::Test
  def setup
    @set = IntegerSet::Set.new(10)
    @size = 10
  end

  def test_size
    assert_equal 10, @set.size
  end

  def test_cannot_insert_non_numbers
    assert_raises(TypeError) { @set.insert('hi') }
    assert_raises(TypeError) { @set.insert(nil) }
    assert_raises(TypeError) { @set.insert(Object.new) }
  end

  def test_initializes_empty
    refute((0..@size).any?(&@set.method(:include?)))
  end

  def test_insert_and_test_for_existence
    @set.insert(0)
    @set.insert(4)
    @set.insert(7)
    assert @set.include?(0)
    assert @set.include?(4)
    assert @set.include?(7)
    refute [1, 2, 3, 5, 6, 8, 9].any?(&@set.method(:include?))
  end

  def test_each
    @set = IntegerSet::Set.new(10)
    @set.insert(0)
    @set.insert(4)
    @set.insert(7)
    results = []
    @set.each do |n|
      results << n
    end
    assert_equal [0, 4, 7], results
  end

  def test_implements_enumerable
    @set.insert(0)
    @set.insert(4)
    @set.insert(7)
    assert_equal([1, 5, 8], @set.map { |x| x + 1 })
  end
end
