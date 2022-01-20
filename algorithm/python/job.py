class Job:
    def __init__(self, name, after, before):
        self.name = name
        self.after = after
        self.before = before

    def __init__(self, key, value):
        self.name = key
        self.after = value['after'] if 'after' in value else []
        self.before = value['before'] if 'before' in value else []

    def __str__(self):
        result_template =  'Job: name=\'{name}\',after=[{after}],before=[{before}]'
        result = result_template.format(name=self.name, after=self.after, before=self.before)
        return result
